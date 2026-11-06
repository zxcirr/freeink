# FreeInk

Free your ink. 自制固件 + BLE 上位机，运行在两块 ZKTEC 电子价签上：

| 板子 | SoC | 屏 | 控制器 | 分辨率 | 颜色 | 状态 |
|---|---|---|---|---|---|---|
| ZKC21V (ZK_EL_OM6626_2.13_V1.1) | OnMicro OM6626 | 2.13" | SSD1680 | 128×250 (可视122) | BWR | ✅ 已调通 |
| ZKC42V | OnMicro OM6626 | 4.2" | JD79668AA (SSD1680 兼容指令集) | 400×300 | BWR | ✅ 已调通 |

通过 BLE 从电脑推送任意图片直接刷屏，完全绕开原厂 App/云服务。
本项目为独立实现，不包含也不链接厂商专有固件代码。

## 仓库结构

```text
firmware/
  src/            OM6626 固件源码 (C)
  gcc/Makefile    构建（需要 OnMicro OM6626 SDK）
  jlink/          J-Link Commander 脚本（备份/烧录/恢复）
host/
  zkc_epd.py      Python BLE 上位机（bleak + Pillow，Windows/Linux 通用）
  examples/       draw.io 图卡模板
docs/
  protocol.md     BLE 传输协议
  HANDOVER.md     工程交接文档（硬件细节、flash 布局、踩坑记录）
```

## 依赖

- [OnMicro OM6626 SDK](https://github.com/OnMicro/OM6626_SDK)（BSD 类许可）。
  默认路径 `../../../OM6626-sdk`，可用 `make SDK_ROOT=/path/to/SDK` 覆盖。
- ARM GNU 工具链（实测 xPack 12.3.1；GCC 14 与 SDK 的 BLE ROM 库链接不兼容）。
- 上位机：Python ≥ 3.10，`pip install -r host/requirements.txt`（bleak、Pillow）。

## 构建固件

```sh
cd firmware/gcc
make BOARD=ZKC21V -j4        # → firmware/build/freeink_ZKC21V.bin
make BOARD=ZKC42V -j4
```

产物链接在 `0x404000`（原厂 recovery 槽位），大小上限 155,648 B。
**不要**全片擦除，**不要**写 `0x400000-0x403FFF` / MBR / 工厂区。
详见 `docs/HANDOVER.md` 的 flash 布局与备份/恢复流程。

## 烧录（J-Link，Windows）

```text
JLink.exe -CommanderScript firmware/jlink/backup_21v.jlink     # 先备份！
JLink.exe -CommanderScript firmware/jlink/flash_21v.jlink
JLink.exe -CommanderScript firmware/jlink/restore_21v.jlink    # 恢复原厂
```

（4.2 用 `_42v` 版本；脚本见 `firmware/jlink/`。）

## 上位机用法

```powershell
python host/zkc_epd.py --name ZKC21V --test              # 测试卡
python host/zkc_epd.py --name ZKC21V --image pic.png     # 任意图片
python host/zkc_epd.py --name ZKC42V --image pic.png     # 4.2 原生横屏
python host/zkc_epd.py --name ZKC21V --clear             # 清屏
python host/zkc_epd.py --name ZKC21V --image x.png --preview out.png
```

ZKC21V 的显示方向补偿（rotate 90 + mirror-y）已内置默认，无需参数；
ZKC42V 原生即正确。协议：双 1bpp 平面 (format 2) + CRC32，见 `docs/protocol.md`。

## 已知边界

- 无 OTA、无深度睡眠（BLE 常开广播）、每次全刷、无图片存储。
- 固件尺寸余量 < 5 KB，扩展功能前需改为 bootloader + app 两段布局。
- 屏型号/极性的验证方法论（先 `--clear` 纯白、等 BUSY 纪律、原厂序列逆向）
  记录在 `docs/HANDOVER.md`。

## 许可

- 本仓库代码：MIT（见 `LICENSE`）。
- OnMicro SDK 与其预编译 BLE 库：随 SDK 的许可，自行下载，不在本仓库内。
- 厂商（InkTag）固件/素材：PolyForm Noncommercial，本仓库不含其任何内容。
