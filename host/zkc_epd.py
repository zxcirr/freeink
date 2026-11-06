#!/usr/bin/env python3
"""
Host-side BLE image uploader for the ZKC21V / ZKC42V custom firmware.

Requirements:
    pip install -r requirements.txt

Examples:
    python zkc_epd.py --test
    python zkc_epd.py --image photo.jpg --rotate 0
    python zkc_epd.py --image photo.jpg --rotate 90 --fit crop
    python zkc_epd.py --clear

The device advertises as ZKC21V or ZKC42V and exposes:

    service  6E400001-B5A3-F393-E0A9-E50E24DCCA9E
    CMD      6E400002-...  write + notify
    DATA     6E400003-...  write (u32 offset LE + payload)

Image format on the wire: two 1bpp planes, row-major, MSB first.
    plane 0: 1 = white, 0 = black
    plane 1: 1 = no red, 0 = red
"""

from __future__ import annotations

import argparse
import asyncio
import struct
import sys
import zlib
from collections.abc import Iterable
from pathlib import Path

try:
    from bleak import BleakClient, BleakScanner
except ImportError:
    print("Missing dependency: pip install -r requirements.txt", file=sys.stderr)
    raise

from PIL import Image, ImageDraw, ImageFont, ImageOps

SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
CMD_UUID = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
DATA_UUID = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

# Older prototype firmware used these byte-swapped UUIDs. Keep accepting
# them so the host works before the corrected firmware is reflashed.
SERVICE_UUID_OLD = "6e400001-a3b5-93f3-e0a9-e50e24dcca9e"
CMD_UUID_OLD = "6e400002-a3b5-93f3-e0a9-e50e24dcca9e"
DATA_UUID_OLD = "6e400003-a3b5-93f3-e0a9-e50e24dcca9e"

CMD_INFO = 0x01
CMD_BEGIN = 0x02
CMD_COMMIT = 0x03
CMD_ABORT = 0x04
CMD_CLEAR = 0x05

EVT_DONE = 0xE2
EVT_ERROR = 0xE6

STATUS_NAMES = {
    0x00: "OK",
    0x01: "INVALID",
    0x02: "BUSY",
    0x03: "CRC",
    0x04: "SIZE",
    0x05: "STATE",
}


class DeviceError(RuntimeError):
    pass


def make_client(address: str) -> BleakClient:
    """Create a client; on Windows force fresh GATT discovery.

    Windows caches the GATT database per device address. After reflashing a
    device, a stale InkTag/previous-firmware cache can hide the new service.
    """
    kwargs = {}
    if sys.platform == "win32":
        kwargs["winrt"] = {"use_cached_services": False}
    return BleakClient(address, **kwargs)


def dump_gatt(client: BleakClient) -> None:
    print("GATT services currently visible to the host:")
    try:
        services = list(client.services)
    except Exception as exc:
        print(f"  <services unavailable: {exc}>")
        return
    if not services:
        print("  <none>")
    for service in services:
        print(f"  service {service.uuid}  {service.description}")
        for char in service.characteristics:
            print(f"    char  {char.uuid}  {char.properties}")


class EpdClient:
    def __init__(self, client: BleakClient):
        self.client = client
        self.responses: asyncio.Queue[bytes] = asyncio.Queue()
        self.seq = 0
        self.max_chunk = 16
        self.service_uuid = ""
        self.cmd_uuid = CMD_UUID
        self.data_uuid = DATA_UUID

    def _on_notify(self, _sender, data: bytearray) -> None:
        self.responses.put_nowait(bytes(data))

    def _resolve_uuids(self) -> None:
        services = list(self.client.services)
        for service in services:
            su = str(service.uuid).lower()
            if su not in (SERVICE_UUID, SERVICE_UUID_OLD):
                continue
            cmd = data = None
            for char in service.characteristics:
                cu = str(char.uuid).lower()
                if cu in (CMD_UUID, CMD_UUID_OLD):
                    cmd = cu
                elif cu in (DATA_UUID, DATA_UUID_OLD):
                    data = cu
            if cmd and data:
                self.service_uuid = su
                self.cmd_uuid = cmd
                self.data_uuid = data
                print(f"Using GATT service {su}")
                return
        dump_gatt(self.client)
        raise DeviceError("EPD service/characteristics not found")

    async def open(self) -> None:
        self._resolve_uuids()
        try:
            await self.client.start_notify(self.cmd_uuid, self._on_notify)
        except Exception:
            dump_gatt(self.client)
            raise
        # Best-effort; some backends negotiate automatically and expose no call.
        exchange = getattr(self.client, "exchange_mtu", None)
        if exchange is not None:
            try:
                await exchange(247)
            except Exception:
                pass
        print(f"Connected: {self.client.address}  MTU={getattr(self.client, 'mtu_size', '?')}")

    async def close(self) -> None:
        if self.client.is_connected:
            try:
                await self.client.stop_notify(self.cmd_uuid)
            except Exception:
                pass
            await self.client.disconnect()

    def _next_seq(self) -> int:
        self.seq = (self.seq + 1) & 0xFF
        return self.seq

    async def _wait_response(self, opcode: int, seq: int, timeout: float) -> bytes:
        deadline = asyncio.get_running_loop().time() + timeout
        while True:
            left = deadline - asyncio.get_running_loop().time()
            if left <= 0:
                raise DeviceError(f"timeout waiting for 0x{opcode | 0x80:02X}")
            data = await asyncio.wait_for(self.responses.get(), left)
            if len(data) >= 3 and data[0] == (opcode | 0x80) and data[1] == seq:
                if data[2] != 0:
                    name = STATUS_NAMES.get(data[2], f"0x{data[2]:02X}")
                    raise DeviceError(f"device rejected 0x{opcode:02X}: {name}")
                return data

    async def command(self, opcode: int, payload: bytes = b"", timeout: float = 15.0) -> bytes:
        seq = self._next_seq()
        packet = bytes((opcode, seq)) + payload
        await self.client.write_gatt_char(self.cmd_uuid, packet, response=True)
        return await self._wait_response(opcode, seq, timeout)

    async def info(self) -> dict:
        data = await self.command(CMD_INFO)
        if len(data) < 11:
            raise DeviceError("short INFO response")
        version = data[3]
        width, height = struct.unpack_from("<HH", data, 4)
        fmt = data[8]
        max_chunk = struct.unpack_from("<H", data, 9)[0]
        if fmt != 2:
            raise DeviceError(f"unsupported device format {fmt}")
        self.max_chunk = max(1, min(240, max_chunk))
        return {
            "version": version,
            "width": width,
            "height": height,
            "format": fmt,
            "max_chunk": self.max_chunk,
        }

    async def clear(self, timeout: float = 60.0) -> None:
        seq = self._next_seq()
        await self.client.write_gatt_char(self.cmd_uuid, bytes((CMD_CLEAR, seq)), response=True)
        await self._wait_response(CMD_CLEAR, seq, 10.0)
        await self._wait_done(seq, timeout)

    async def upload(self, payload: bytes, timeout: float = 120.0) -> None:
        info = await self.info()
        fmt = info["format"]
        expected = 2 * ((info["width"] + 7) // 8) * info["height"]
        if fmt != 2:
            raise DeviceError(f"unsupported device format {fmt}")
        if len(payload) != expected:
            raise DeviceError("image size does not match device dimensions")
        total = len(payload)
        crc = zlib.crc32(payload) & 0xFFFFFFFF
        begin = struct.pack(
            "<HHBII",
            info["width"],
            info["height"],
            fmt,
            total,
            crc,
        )
        seq = self._next_seq()
        await self.client.write_gatt_char(self.cmd_uuid, bytes((CMD_BEGIN, seq)) + begin, response=True)
        reply = await self._wait_response(CMD_BEGIN, seq, 10.0)
        max_chunk = struct.unpack_from("<H", reply, 3)[0] if len(reply) >= 5 else self.max_chunk
        max_chunk = max(1, min(240, max_chunk))
        print(f"Uploading {total} bytes, chunk={max_chunk}, crc=0x{crc:08X}")

        sent = 0
        next_report = 10
        while sent < total:
            chunk = payload[sent : sent + max_chunk]
            packet = struct.pack("<I", sent) + chunk
            await self.client.write_gatt_char(self.data_uuid, packet, response=True)
            sent += len(chunk)
            pct = sent * 100 // total
            if pct >= next_report:
                print(f"  {pct}% ({sent}/{total})")
                next_report += 10

        seq = self._next_seq()
        await self.client.write_gatt_char(
            self.cmd_uuid, bytes((CMD_COMMIT, seq)) + struct.pack("<I", crc), response=True
        )
        await self._wait_response(CMD_COMMIT, seq, 10.0)
        print("Committed; waiting for panel refresh...")
        await self._wait_done(seq, timeout)
        print("Panel refresh done.")

    async def _wait_done(self, seq: int, timeout: float) -> None:
        deadline = asyncio.get_running_loop().time() + timeout
        while True:
            left = deadline - asyncio.get_running_loop().time()
            if left <= 0:
                raise DeviceError("timeout waiting for display completion")
            data = await asyncio.wait_for(self.responses.get(), left)
            if len(data) >= 3 and data[0] == EVT_DONE and data[1] == seq:
                if data[2] != 0:
                    raise DeviceError(f"display failed: 0x{data[2]:02X}")
                return
            if len(data) >= 3 and data[0] == EVT_ERROR:
                raise DeviceError(f"device error: 0x{data[2]:02X}")


def _is_red(r: int, g: int, b: int, threshold: int) -> bool:
    return r >= threshold and r > g + 25 and r > b + 25


def quantize_bwr(
    image: Image.Image,
    *,
    threshold: int = 128,
    red_threshold: int = 110,
    dither: bool = False,
) -> tuple[bytes, bytes]:
    """Return (black_plane, red_plane), MSB first, 1=white / 1=no-red."""
    image = image.convert("RGB")
    w, h = image.size
    if w % 8:
        raise ValueError("image width must be a multiple of 8")
    plane_len = (w // 8) * h
    black = bytearray(b"\xFF" * plane_len)
    red = bytearray(b"\xFF" * plane_len)
    px = image.load()

    # Optional simple ordered dithering of the luminance. Red is thresholded
    # separately; this is intentionally a small, dependency-free prototype.
    bayer = (
        (0, 8, 2, 10),
        (12, 4, 14, 6),
        (3, 11, 1, 9),
        (15, 7, 13, 5),
    )

    for y in range(h):
        row = y * (w // 8)
        for x in range(w):
            r, g, b = px[x, y]
            if _is_red(r, g, b, red_threshold):
                # red pixel: black plane white, red plane 0
                byte = row + (x >> 3)
                bit = 1 << (7 - (x & 7))
                black[byte] |= bit
                red[byte] &= ~bit
                continue

            lum = (r * 299 + g * 587 + b * 114) // 1000
            if dither:
                bias = (bayer[y & 3][x & 3] - 7.5) * 4
                is_black = lum + bias < threshold
            else:
                is_black = lum < threshold
            if is_black:
                black[row + (x >> 3)] &= ~(1 << (7 - (x & 7)))
            # white leaves black=1, red=1

    return bytes(black), bytes(red)


def make_test_image(width: int, height: int, name: str) -> Image.Image:
    img = Image.new("RGB", (width, height), "white")
    d = ImageDraw.Draw(img)
    d.rectangle((0, 0, width - 1, height - 1), outline="black", width=3)
    # Corner marker: an asymmetric "F" (shows mirror) in a black box at top-left.
    d.rectangle((8, 8, width // 3, height // 6), fill="black")
    try:
        font_f = ImageFont.truetype("DejaVuSans-Bold.ttf", max(18, height // 8))
        font_s = ImageFont.truetype("DejaVuSans.ttf", max(10, height // 22))
    except Exception:
        font_f = ImageFont.load_default()
        font_s = font_f
    d.text((14, 10), "F", fill="white", font=font_f)
    # Axis labels so rotation/mirror can be read directly off the panel.
    d.text((width // 2 - 20, 4), "TOP", fill="black", font=font_s)
    d.text((width // 2 - 34, height - 14), "BOTTOM", fill="black", font=font_s)
    d.text((width - 14, height // 2 - 6), "R", fill="black", font=font_s)
    d.text((4, height // 2 - 6), "L", fill="black", font=font_s)
    # Red block top-right (right edge).
    d.rectangle((width * 2 // 3, 8, width - 8, height // 4), fill=(200, 0, 0))
    d.rectangle((40, 100, 54, 140), fill="black")
    d.polygon(
        [(56, 104), (78, 120), (56, 136)],
        fill="black",
    )
    d.text((width // 10, height * 2 // 3), f"{name}\n{width}x{height}", fill="black", font=font_s)
    return img


def prepare_image(
    source: Path | None,
    width: int,
    height: int,
    *,
    name: str,
    rotate: int,
    mirror_x: bool,
    mirror_y: bool,
    fit: str,
) -> Image.Image:
    if source is None:
        img = make_test_image(height, width, name) if rotate in (90, 270) \
            else make_test_image(width, height, name)
    else:
        img = Image.open(source).convert("RGB")
    if rotate:
        img = img.rotate(-rotate, expand=True)
    if mirror_x:
        img = ImageOps.mirror(img)
    if mirror_y:
        img = ImageOps.flip(img)
    if fit == "stretch":
        return img.resize((width, height), Image.Resampling.LANCZOS)
    # crop to fill
    return ImageOps.fit(img, (width, height), method=Image.Resampling.LANCZOS)


async def find_device(name: str | None, address: str | None, timeout: float) -> BleakClient:
    if address:
        return make_client(address)
    print(f"Scanning for {name or 'ZKC21V/ZKC42V'} ...")
    devices = await BleakScanner.discover(
        timeout=timeout, service_uuids=[SERVICE_UUID, SERVICE_UUID_OLD]
    )
    if not devices:
        # Some platforms omit service UUIDs from scan results; fall back to names.
        devices = await BleakScanner.discover(timeout=timeout)
    matches = []
    for d in devices:
        if name and d.name and name.lower() in d.name.lower():
            matches.append(d)
        elif not name and d.name and (d.name.startswith("ZKC21V") or d.name.startswith("ZKC42V")):
            matches.append(d)
    if not matches:
        raise DeviceError("device not found; check power/name and try --address")
    if len(matches) > 1:
        print("Multiple candidates:")
        for d in matches:
            print(f"  {d.address}  {d.name}")
        raise DeviceError("use --address to select one")
    print(f"Found {matches[0].address}  {matches[0].name}")
    return make_client(matches[0].address)


async def async_main(args: argparse.Namespace) -> int:
    client = await find_device(args.name, args.address, args.scan_timeout)
    await client.connect()
    epd = EpdClient(client)
    try:
        await epd.open()
        info = await epd.info()
        print(
            f"Device: {info['width']}x{info['height']} format={info['format']} "
            f"max_chunk={info['max_chunk']}"
        )
        if args.clear:
            print("Clearing panel...")
            await epd.clear(timeout=args.timeout)
            return 0

        visible_w = 122 if (info["width"], info["height"]) == (128, 250) else info["width"]

        # ZKC21V (128x250 BWR): the panel's source/gate scan mirrors the
        # image vertically, and portrait content reads sideways on the tag.
        # Verified default: landscape art -> rotate 90 CW + flip vertical.
        zkc21 = (info["width"], info["height"]) == (128, 250)
        comp = zkc21
        rotate = args.rotate
        mirror_x = args.mirror_x
        mirror_y = args.mirror_y
        if rotate is None:
            rotate = 0
            if comp and args.image is not None:
                with Image.open(args.image) as probe:
                    if probe.width > probe.height:
                        rotate = 90
            elif comp and args.image is None:
                rotate = 90  # make the test card landscape like real use
        if mirror_x is None:
            mirror_x = False
        if mirror_y is None:
            mirror_y = comp
        if comp and (rotate or mirror_y):
            print("Using ZKC21V default orientation (rotate=%d, mirror-y=%s)"
                  % (rotate, mirror_y))

        img = prepare_image(
            args.image,
            visible_w,
            info["height"],
            name=args.name or "ZKC",
            rotate=rotate,
            mirror_x=mirror_x,
            mirror_y=mirror_y,
            fit=args.fit,
        )
        if visible_w != info["width"]:
            canvas = Image.new("RGB", (info["width"], info["height"]), "white")
            canvas.paste(img, (0, 0))
            img = canvas
        black, red = quantize_bwr(
            img,
            threshold=args.threshold,
            red_threshold=args.red_threshold,
            dither=args.dither,
        )
        if args.preview:
            # Show the quantized result in a preview PNG.
            preview = Image.new("RGB", img.size, "white")
            p = preview.load()
            w = img.size[0]
            for y in range(img.size[1]):
                for x in range(w):
                    idx = y * (w // 8) + (x >> 3)
                    bit = 1 << (7 - (x & 7))
                    if not (red[idx] & bit):
                        p[x, y] = (200, 0, 0)
                    elif not (black[idx] & bit):
                        p[x, y] = (0, 0, 0)
            preview.save(args.preview)
            print(f"Preview: {args.preview}")
        await epd.upload(black + red, timeout=args.timeout)
        return 0
    finally:
        await epd.close()


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--name", default=None, help="BLE name prefix (default: auto-detect ZKC21V/ZKC42V)")
    p.add_argument("--address", help="BLE address; skips scanning")
    p.add_argument("--image", type=Path, help="input image (PNG/JPEG/...)")
    p.add_argument("--test", action="store_true", help="send a generated test pattern")
    p.add_argument("--clear", action="store_true", help="clear the panel")
    p.add_argument("--rotate", type=int, choices=(0, 90, 180, 270), default=None,
                   help="rotate source CCW? (negative in code); default: auto for ZKC21V")
    p.add_argument("--mirror-x", action="store_true", default=None)
    p.add_argument("--mirror-y", action="store_true", default=None)
    p.add_argument("--fit", choices=("crop", "stretch"), default="crop")
    p.add_argument("--threshold", type=int, default=128, help="black/white luminance threshold")
    p.add_argument("--red-threshold", type=int, default=110)
    p.add_argument("--dither", action="store_true", help="ordered dithering for grayscale")
    p.add_argument("--preview", type=Path, help="save a quantized preview PNG")
    p.add_argument("--scan-timeout", type=float, default=8.0)
    p.add_argument("--timeout", type=float, default=120.0, help="panel refresh timeout")
    args = p.parse_args()

    if args.test and args.image:
        p.error("use either --test or --image")
    if args.test:
        args.image = None
    elif args.image is None and not args.clear:
        args.test = True

    try:
        return asyncio.run(async_main(args))
    except (DeviceError, OSError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        return 130


if __name__ == "__main__":
    raise SystemExit(main())
