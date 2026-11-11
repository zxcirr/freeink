# ZKC EPD BLE protocol

## GATT

```text
Service  6E400001-B5A3-F393-E0A9-E50E24DCCA9E
CMD      6E400002-B5A3-F393-E0A9-E50E24DCCA9E   write + notify
DATA     6E400003-B5A3-F393-E0A9-E50E24DCCA9E   write (offset + chunk)
```

The host must enable notifications on CMD. All multi-byte values are
little-endian.

## CMD packet

```text
[opcode, seq, payload...]
```

The device replies on CMD:

```text
[opcode | 0x80, seq, status, payload...]
```

Status:

```text
0x00 OK
0x01 INVALID
0x02 BUSY
0x03 CRC
0x04 SIZE
0x05 STATE
```

## Commands

### 0x01 INFO

No payload. Reply:

```text
[0x81, seq, 0, version, width_lo, width_hi, height_lo, height_hi,
 format, max_chunk_lo, max_chunk_hi]
```

`format = 2` means two 1bpp planes (BWR). `max_chunk` is the maximum number of
image bytes per DATA write (the DATA payload is `4 + max_chunk` bytes).

### 0x02 BEGIN

Payload:

```text
width u16, height u16, format u8, total u32, crc32 u32
```

`total` is the sum of both planes. `crc32` is zlib/IEEE CRC-32 over the
concatenated planes. Reply:

```text
[0x82, seq, 0, max_chunk_lo, max_chunk_hi]
```

### DATA characteristic

```text
offset u32, image bytes...
```

Offsets must be sequential starting at 0. The host should use the
`max_chunk` from INFO. The firmware accepts the chunk only if
`offset == received_so_far`.

### 0x03 COMMIT

Payload:

```text
crc32 u32
```

Reply `[0x83, seq, 0]` immediately, then after the panel refresh:

```text
[0xE2, seq, 0]
```

The host should wait for `0xE2` (up to 30 s for the 4.2" full refresh).

### 0x04 ABORT

No payload. Reply `[0x84, seq, 0]`.

### 0x05 CLEAR

No payload. Reply `[0x85, seq, 0]`, then `[0xE2, seq, 0]` after the clear.

## Image format 2

Two planes, row-major, MSB first, width padded to a multiple of 8:

```text
plane 0 (black): 1 = white, 0 = black
plane 1 (red):   1 = no red, 0 = red
```

Plane size = `((width + 7) / 8) * height`. For the 4.2" panel both planes are
15,000 bytes (30,000 total); for the 2.13" panel both planes are 4,000 bytes
(8,000 total).

The host quantizes RGB to BWR, packs the two planes, computes CRC-32 over
`plane0 + plane1`, and sends them in order.

## Example

```text
CMD  01 01                         -> 81 01 00 01 90 01 FA 00 02 F0 00
CMD  02 02 90 01 FA 00 02 30 75 00 00 <crc32> -> 82 02 00 F0 00
DATA 00 00 00 00 <240 bytes>
DATA F0 00 00 00 <240 bytes>
...
CMD  03 03 <crc32>                 -> 83 03 00
                                     E2 03 00
```
