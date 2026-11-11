# 硬件、烧录与备份

这份文档回答三个问题：手里的板子是什么、flash 里什么能动什么不能动、怎么备份/烧录/恢复。
日常发图操作见 [WINDOWS.md](WINDOWS.md)；屏幕驱动怎么点亮的、坏了怎么查，见 [BRINGUP.md](BRINGUP.md)。

现有固件已在两块板上点亮，后续会整体重写；换的是软件架构，本文的地址布局和操作边界不变。

## 1. 板卡档案

| | ZKC21V | ZKC42V |
|---|---|---|
| 主控 | OM6626 | OM6626（OM ISP 串口工具与 J-Link 双重确认） |
| 主板丝印 | `ZK_EL_OM6626_2.13_V1.1` | 未拆开确认 |
| 屏幕 | 2.13" 黑白红，128×250（可视 122） | 4.2" 黑白红，400×300 |
| 面板控制器 | SSD1680 兼容指令集（当前驱动按此实现，实机点亮） | JD79668AA（SSD1680 兼容指令集，实机点亮） |
| RGB LED | 有（GPIO4/18/17） | 无，板上没焊（`BOARD_HAS_RGB_LED=0`） |

两块板共用同一组 EPD 引脚、同一条 BLE 协议（format 2 双平面），固件用 `BOARD` 宏区分，一次 `make` 出两份 bin。

4.2 的屏与控制器是靠实测行为认定的，FPC 型号没查过。换屏、换批次、换板时，先走
[BRINGUP.md](BRINGUP.md) 第 4 节的排查流程，再动驱动。

## 2. 引脚与指示灯

EPD 引脚（两板相同）。SPI1，3-wire（面板只写），Mode 0，MSB first，8 MHz：

| 信号 | GPIO | 备注 |
|---|---|---|
| SCK | 15 | SPI1_SCK |
| MOSI | 21 | SPI1_DO |
| CS | 22 | SPI1_CS |
| DC | 20 | 命令/数据 |
| RST | 16 | 复位 |
| BUSY | 3 | 输入，高电平 = 忙 |
| EN | 7 | 输出，高电平有效，面板电源 |

面板电源策略由 `EPD_EN_ALWAYS_ON` 控制（`board.h`）：

- `1`（默认）：首次上电后 EN 保持高，两次刷新之间不断电。这是两块屏验证过的行为，代价是面板静态漏电流。
- `0`：每次刷新后断 EN，下次 `epd_init()` 重新上电，并等 20 ms 让升压电路稳定。省电，但面板若丢了深睡锁存状态可能不再更新——上场前先台架验证。

无论哪种取值，EN 的第一次置高都发生在 `board_init()` 里。运行中途再开电源，升压电容的浪涌会把总线拉出毛病（PM 事故的附带结论，见 [BRINGUP.md](BRINGUP.md) 第 5 节）。

RGB LED（仅 ZKC21V），低电平点亮：红 GPIO4、绿 GPIO18、蓝 GPIO17。

| 灯语 | 含义 |
|---|---|
| 蓝灯 500 ms 闪 | 广播中（`LED_IDLE_BLINK=1` 时才有） |
| 绿灯常亮 | 已连接 |
| 青灯快闪 | 接收图片 |
| 红灯常亮 | 刷屏中 |
| 绿灯亮 1 s 后恢复 | 操作成功 |
| 红灯快闪后恢复 | 协议/CRC/传输错误 |

`LED_IDLE_BLINK=0`（默认）时广播期间灯全灭，板子干活时才亮——省下平均 1–2 mA。

GPIO4 兼职芯片 BOOT 脚：上电复位瞬间把它持续拉低（红灯点亮就等于拉低）会改变启动行为。
所以上电与复位路径上红灯被显式保持为熄灭（GPIO 高）。

## 3. Flash 布局

| 地址 | 内容 | 操作边界 |
|---|---|---|
| 0x400000–0x403FFF | protected boot / CPFT / 工厂校准 | 不写不擦，且没有备份来源 |
| 0x404000–0x429FFF | 第一级槽（155,648 B） | 自制固件的法定居住地 |
| 0x42A000–0x478000 | 出厂 app | 从未被覆盖 |
| 0x480000–0x481FFF | runtime MBR / 保留区 | 不碰 |
| 0x4F0000–0x4F1FFF | NVDS（当前固件在使用） | 备份里为空白 |
| 0x4F9000–0x4FCFFF | 空白 | — |
| 0x4FD000–0x4FFFFF | 出厂 NVDS / BKUP / CONF | 从未被覆盖 |

两种布局形态：

**单镜像**（默认 `make`）：app 直接链接在 0x404000，占满第一级槽。

**boot v0 + 双槽**（`make SLOT=A` 或 `SLOT=B`）：

```text
0x404000  boot v0     64 KB
0x414000  app slot A  192 KB
0x444000  app slot B  192 KB
```

boot v0 依次检查 A、B 槽的向量表（栈指针、复位向量、非空白条目），跳进第一个像样的槽；
两个都坏则红灯快闪呼救。BLE DFU 与换槽记录留给 v1。

尺寸账：槽上限 155,648 B，当前 2.1 bin 151,832 B（余 3,816 B）、4.2 bin 151,352 B（余 4,296 B）。
单镜像再塞功能就爆槽，出路就是双槽布局——boot → 重定位 app 的迁移链路已经打通。

## 4. 构建

工具链：xPack ARM GNU 12.3.1。Debian 仓库的 GCC 14 与 SDK 的 BLE ROM 库链接失败，不要用。

```sh
export PATH=/path/to/xpack-arm-none-eabi-gcc-12.3.1-1.2/bin:$PATH
cd firmware/gcc
make BOARD=ZKC21V -j4     # → firmware/build/freeink_ZKC21V.bin
make BOARD=ZKC42V -j4     # → firmware/build/freeink_ZKC42V.bin
```

| make 变量 | 作用 |
|---|---|
| `BOARD=ZKC21V / ZKC42V` | 选板（引脚、分辨率、LED 能力） |
| `SLOT=A / B` | 链接到 0x414000 / 0x444000，产物名加 `_slotA`/`_slotB`，配合 boot v0 使用 |
| `PM=1` | 启用主循环 PM 睡眠（宏 `PM_SLEEP_ENABLE`），默认关。PM 与刷屏的恩怨见 [BRINGUP.md](BRINGUP.md) 第 5 节 |
| `EPDDBG=1` | DONE 通知多带 12 字节调试尾巴（等 BUSY 超时位图、耗时、BUSY 电平、SPI 错误计数） |
| `SDK_ROOT=…` | 默认用仓库内 `sdk/`（BSD-3，出处见 `sdk/NOTICE`）；也可指向完整官方 SDK（OnMicro/OM6626_SDK） |

bootloader 单独构建：`cd firmware/boot/gcc && make -j4`，产物在 `firmware/boot/build/freeink_boot.bin`。
`flash_21v_ab.bat` 按 `firmware/build/freeink_boot.bin` 取文件，用前拷过去。

参考边界：EPD-nRF5（GPL-3.0）只作资料查阅，其代码不能进闭源固件。

## 5. 备份（每块板第一次动手前）

工具：`tools\win\backup_21v.bat` / `backup_42v.bat`。
动作：halt → `savebin backup\<板名>-app-0x404000.bin, 0x404000, 0xFC000` → 复位。
读出范围 0x404000–0x4FFFFF，共 1,032,192 字节。

- 备份从 0x404000 开始，**不含** 0x400000–0x403FFF。protected boot / CPFT / 工厂校准没有已知的再获取手段，全片擦除一次它们就永久丢失——这是"永不全片擦除"存在的全部理由。
- 2.1 的出厂备份做过核验：S-record 连续覆盖 0x404000–0x4FFFFF、无空洞、校验和全部正确；0x404000 处的 recovery 镜像与出厂 recovery.bin、0x42A000 处的 app 镜像与出厂 OTA application.bin 的 SHA256 逐一吻合。
- 2.1 另有一份只含第一级槽的 `restore-appslot-0x404000.bin`（155,648 B），Windows 端 `backup\` 内留有副本，`restore_21v.bat` 用它。
- 4.2 的恢复源是 `backup_42v.bat` 产出的 `zkc42v-app-0x404000.bin`。

## 6. 烧录

一键脚本会自动找 J-Link（PATH → Program Files → 手动输入一次，路径记进 `tools\win\jlink.cfg`）：

| 脚本 | 写入内容 |
|---|---|
| `flash_21v.bat` / `flash_42v.bat` | 单镜像 → 0x404000 |
| `flash_21v_ab.bat` | boot v0 → 0x404000，app → 0x414000（slot A） |
| `flash_21v_pmtest.bat` / `flash_42v_pmtest.bat` | PM 候选构建 → 0x404000 |

手动 J-Link Commander（与脚本等价，多一步校验）：

```text
device OM6626
si SWD
speed 4000
connect
halt
loadbin firmware/build/freeink_ZKC21V.bin, 0x404000
verifybin firmware/build/freeink_ZKC21V.bin, 0x404000
r
g
exit
```

写入边界：只写第 3 节布局中标为"自制固件居住地"的地址。
0x400000–0x403FFF、0x480000–0x481FFF、0x4F9000+ 一个字节都不写。
erase 类命令先想清楚作用范围再敲——备份不含 0x400000–0x403FFF，擦了就没有恢复来源。

## 7. 恢复

| 场景 | 操作 |
|---|---|
| 回出厂固件（2.1） | `restore_21v.bat`：出厂 recovery 镜像写回 0x404000（155,648 B）。0x42A000 起的出厂 app 从未被覆盖，第一级槽复原即整机复原 |
| 回出厂固件（4.2） | `restore_42v.bat`：`backup\zkc42v-app-0x404000.bin` 写回 0x404000。前置条件：做过 `backup_42v.bat` |
| 恢复全部已备份范围 | `loadbin backup-0x404000.bin, 0x404000`——起点是 0x404000，不是 0x400000 |

## 8. 板子卡死了

`probe_21v.bat`（`hang_probe.jlink`）：attach → halt → 打印 PC/LR。
把 `PC = 0x…` 那行拿来对着 `firmware/build/*.map` 符号化，通常一眼能看出卡在哪个函数
（SPI 时钟域事故就是这么定位的，见 [BRINGUP.md](BRINGUP.md) 第 5 节）。
看完 power-cycle 复位即可，attach 本身不破坏现场。
