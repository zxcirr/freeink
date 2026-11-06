# FreeInk

> Free your ink — 让电子价签听你的话，而不是听云端的。

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/MCU-OnMicro%20OM6626-blue)](https://github.com/OnMicro/OM6626_SDK)
[![Python](https://img.shields.io/badge/Python-3.10%2B-green)](https://python.org)
[![BLE](https://img.shields.io/badge/transport-BLE%205.0-informational)]()

FreeInk 是运行在两块钱捡来的 ZKTEC 电子价签（OM6626 主控）上的**自制固件 + PC 上位机**：把任意图片通过蓝牙直接刷到墨水屏上，不依赖原厂 App、不连互联网、不加水印。适合想把退役/廉价价签改造成桌面日历、信息牌、电子相册的硬件玩家和开源社区开发者。

## 支持硬件

| 板子 | SoC | 屏幕 | 控制器 | 分辨率 | 颜色 |
|---|---|---|---|---|---|
| ZKC21V（`ZK_EL_OM6626_2.13_V1.1`） | OM6626 | 2.13" | SSD1680 | 128×250（可视 122） | 黑/白/红 |
| ZKC42V | OM6626 | 4.2" | JD79668AA | 400×300 | 黑/白/红 |

两块板共用同一套引脚与协议，固件按 `BOARD` 宏区分，一次 `make` 出两份 bin。

## 特性

- **纯 BLE 链路**：自定义 GATT 协议（分块传输 + CRC32 + 确认刷新），断传可 ABORT 重传
- **两平面 1bpp 直传**：设备端零解码、零计算，收到即灌屏，固件仅 ~148 KB
- **上位机一条命令上图**：自动裁剪 / 旋转 / 方向补偿 / 阈值量化 / 有序抖动 / 本地预览
- **日历卡片生成器**：农历、干支、十二值神宜忌、节气、实时天气，一次生成上屏
- **J-Link 一键脚本**：备份 → 烧录 → 恢复原厂，全程只动 `0x404000` 一个槽位
- **两块板独立身份**：各自的 BLE 名与固定随机地址，同场多台互不串扰

## 仓库结构

```text
firmware/
  src/            固件源码（C，~1100 行）
  gcc/Makefile    构建入口
  jlink/          J-Link Commander 脚本
host/
  zkc_epd.py      BLE 上位机（发图 / 清屏 / 测试卡）
  calendar_card.py日历卡片生成器
  examples/       图卡模板与成品示例
docs/
  protocol.md     BLE 协议规范
  HANDOVER.md     工程交接与踩坑记录
tools/win/        Windows 一键 .bat
```

## 安装

### 上位机（Windows / Linux / macOS）

```bash
git clone https://github.com/zxcirr/freeink.git
cd freeink
python -m pip install -r host/requirements.txt   # bleak + Pillow + lunar-python
```

Windows 用户也可以直接双击 `tools/win/setup.bat`。

### 固件构建（可选，改固件才需要）

1. [xPack ARM GNU 12.3.1](https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack)（14.x 与 SDK 的 BLE ROM 库不兼容）
2. [OnMicro OM6626 SDK](https://github.com/OnMicro/OM6626_SDK)，放在仓库同级目录，或 `make SDK_ROOT=/path/to/SDK`

```bash
cd firmware/gcc
export PATH=/path/to/xpack-arm-none-eabi-gcc-12.3.1-1.2/bin:$PATH
make BOARD=ZKC21V -j4
make BOARD=ZKC42V -j4
# → firmware/build/freeink_ZKC21V.bin / freeink_ZKC42V.bin
```

## 快速开始（5 分钟）

以 4.2" ZKC42V 为例，需要一次 J-Link SWD 烧录：

1. **备份原厂固件**（新板子必做！）：双击 `tools/win/backup_42v.bat`
2. **烧入 FreeInk**：双击 `tools/win/flash_42v.bat`
3. **清屏验证**：`tools/win/clear.bat 42` —— 屏应变为纯白
4. **发图**：`tools/win/send.bat 42 任意图片.png`

之后上屏不再需要 J-Link，蓝牙即发即刷。

> 烧录铁律：只写 `0x404000`；**永不**全片擦除，**永不**碰
> `0x400000–0x403FFF`（引导/工厂校准）、MBR 与 `0x4F9000+`。
> 详见 `docs/HANDOVER.md`。

## 使用示例

```bash
# 任意图片 → 上屏（host 自动裁剪缩放）
python host/zkc_epd.py --name ZKC42V --image photo.jpg

# 先在本地生成"量化后屏上会显示什么"的预览图，再决定是否发送
python host/zkc_epd.py --name ZKC42V --image photo.jpg --preview out.png

# 2.1" 内置了方向补偿（rotate 90 + mirror-y），直接发即可
python host/zkc_epd.py --name ZKC21V --image card.png

# 照片类内容加有序抖动，灰度过渡更好
python host/zkc_epd.py --name ZKC21V --image photo.jpg --dither

# 测试卡 / 清屏
python host/zkc_epd.py --name ZKC21V --test
python host/zkc_epd.py --name ZKC42V --clear

# 多设备同场时按地址点名
python host/zkc_epd.py --address A5:5A:00:00:42:C0 --image x.png
```

日历卡片（今天示例输出 `examples/calendar_2026-11-06.png`）：

```bash
# 生成整月日历卡（默认苏州天气）：日期网格+农历、干支、宜忌、节气、第几周
python host/calendar_card.py
# 指定日期 / 换城市 / 不要天气 / 自定义输出
python host/calendar_card.py --date 2027-01-01 --city Shanghai --no-weather -o newyear.png
# 直接上屏
python host/zkc_epd.py --name ZKC42V --image examples/calendar_2026-11-06.png
```

中文字体自动探测顺序：`~/.fonts/NotoSansCJKsc-*` → Windows 微软雅黑 →
Linux Noto/文泉驿。Linux 下推荐：

```bash
mkdir -p ~/.fonts && curl -LO https://github.com/googlefonts/noto-cjk/raw/main/Sans/OTF/SimplifiedChinese/NotoSansCJKsc-Regular.otf && mv NotoSansCJKsc-Regular.otf ~/.fonts/
```

## 配置 / API

### `zkc_epd.py` 参数

| 参数 | 说明 |
|---|---|
| `--name` / `--address` | BLE 广播名（ZKC21V/ZKC42V）或直连地址 |
| `--image` | 输入图片（PNG/JPEG/任意 Pillow 可读格式） |
| `--test` / `--clear` | 内置测试卡 / 清屏 |
| `--rotate {0,90,180,270}` | 旋转（覆盖 ZKC21V 默认补偿） |
| `--mirror-x` / `--mirror-y` | 镜像 |
| `--threshold` / `--red-threshold` | 黑白亮度阈值 / 红色判定阈值 |
| `--dither` | Bayer 有序抖动 |
| `--preview PATH` | 输出量化预览 PNG（不上屏时可用于校验） |
| `--fit {crop,stretch}` | 等比裁剪填充 / 直接拉伸 |

### 图像数据格式（线上格式）

两张 1bpp 平面拼接，MSB 在左，行优先：

```text
plane 0（黑）: 1 = 白, 0 = 黑
plane 1（红）: 1 = 非红, 0 = 红      # 固件写 0x26 前取反
```

2.13": 4000 + 4000 = 8000 字节；4.2": 15000 + 15000 = 30000 字节。

### BLE 协议

NUS 风格服务：`6E400001-...`；CMD 特征（write+notify）承载
`INFO/BEGIN/COMMIT/ABORT/CLEAR`，DATA 特征承载 `u32 offset + ≤240B` 分块；
全 CRC32 校验。完整规范见 **[docs/protocol.md](docs/protocol.md)**。

### 引脚（两块板相同）

```text
SCK GPIO15  MOSI GPIO21  CS GPIO22  DC GPIO20  RST GPIO16  BUSY GPIO3  EN GPIO7
```

## 已知限制 / Roadmap

- 无 OTA（升级需 J-Link）；无深度睡眠，BLE 常开广播 <!-- TODO: 低功耗 -->
- 每次全刷，无局部刷新
- 固件距槽位上限 155,648 B 余量不足 5 KB，加功能前需拆 bootloader
- 图片不落盘存储，掉电即失 <!-- TODO: flash 图片槽 -->

## 贡献指南

欢迎 PR！建议流程：

1. Fork 后基于 `master` 开分支；commit 信息说清"为什么"而不只是"改了什么"
2. 改固件：`make BOARD=ZKC21V && make BOARD=ZKC42V` 两个目标都必须通过
3. 改上位机：保持 `python host/zkc_epd.py --preview` 在无蓝牙设备时可独立验证转换结果
4. 新板型/新屏：在 `src/board.h` 加 `BOARD_*` 分支 + 新增 `src/epd_*.c` 驱动，
   驱动务必遵守本项目的等 BUSY 纪律（任何 `0x20` 激活后必须轮询到空闲才能写下一条命令）
5. 提 issue 时请附：板子丝印、屏 FPC 丝印、`--preview` 对比图

大改动建议先开 issue 讨论。代码风格：C 用 K&R、4 空格；Python 无第三方 lint 强制要求。

## 许可证

- 本项目代码（firmware/src、host、tools、docs 中原创内容）：**MIT**（见 [LICENSE](LICENSE)）
- OnMicro OM6626 SDK 及其预编译 BLE ROM 库：随 SDK 自带许可，需自行获取，本仓库不含
- 日历功能依赖 `lunar-python`（MIT）与 `Pillow`/`bleak`，均为标准 PyPI 开源包

## 免责声明

本项目研究/ Hobby 性质。刷机会失去保修；请在可控可恢复的板子上实验，
动手前先做完整 flash 备份。因使用本项目导致的砖机、墨水污染等后果自负。
