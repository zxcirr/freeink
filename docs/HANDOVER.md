# ZKC21V / ZKC42V 自制固件交接文档

更新时间：2026-11-06  
状态：**两块屏均已调通；工程已独立为 FreeInk 仓库（git，master 分支）。**

---

## 1. 项目目标

在 OM6626 电子价签上运行自己写的固件：

- 通过 BLE 从电脑/手机上传黑白红图片
- 直接驱动屏幕，不依赖 InkTag WebUI
- 不显示 InkTag 的 `InkTag Free` 标识
- 后续扩展到 4.2 寸 ZKC42V

---

## 2. 当前状态

| 项目 | 状态 | 说明 |
|---|---|---|
| 2.1 寸 BLE 广播 `ZKC21V` | ✅ 已验证 | 可被 Windows 扫描到 |
| 2.1 寸 BLE 连接 | ✅ 已验证 | MTU 247，GATT 服务可发现 |
| 2.1 寸图片传输 | ✅ 已验证 | 8000 字节，分块、CRC、COMMIT 均通过 |
| 2.1 寸触发刷屏 | ✅ 已验证 | `--clear` 和 `--test` 都会触发屏幕刷新过程 |
| 2.1 寸最终显示 | ✅ 已解决 | 花屏根因：0x20 激活后未等 BUSY，RAM 写入被忽略。现已按完整初始化+等 BUSY 流程修复 |
| 2.1 寸方向/镜像 | ✅ 已解决 | 该屏需 rotate 90 + mirror-y；已在 host 里做成 ZKC21V 默认补偿 |
| 2.1 寸无 InkTag logo | ⏳ 未确认 | 需要显示正常后用 `--clear` 验证 |
| 2.1 寸 RGB LED | ⚠️ 已实现 | 代码和状态机已完成，完整灯效仍需确认 |
| 4.2 寸 ZKC42V | ✅ 显示已调通 | 控制器为 JD79668AA（SSD1680 兼容指令集）配 BWR 三色彩屏；协议与 2.1 相同（format 2 双平面）；`--clear`/`--test` 无参数即正确显示（原生横屏，无需旋转/镜像补偿）；板上无 RGB LED（BOARD_HAS_RGB_LED=0 编译屏蔽） |
| OTA / 低功耗 / 局刷 / 图片存储 | ❌ 未实现 | 当前只做 BLE 传图 + 全刷 |

**重要：花屏原因尚未定位。此前针对花屏做过的参数尝试没有解决问题，不应把当前 `epd_ssd1680.c` 里的初始化/刷新参数当作可靠结论。**

---

## 3. 硬件信息

### 2.1 寸 ZKC21V

- 主控：OM6626
- 主板丝印：`ZK_EL_OM6626_2.13_V1.1`
- 屏幕：2.13 寸，128×250，黑白红（BWR），可见宽度 122
- 屏幕控制器：InkTag 配置标为 SSD1680
- 已刷过 InkTag 0.2.0，并能正常启动

### 4.2 寸 ZKC42V

- 主控：OM6626（已通过 OM ISP 串口工具和 J-Link 确认）
- 产品型号：ZKC42V
- 未拆开确认主板丝印和屏幕 FPC
- InkTag 文档里 ZKC42V 原屏为 400×300 BWR、UC8276
- **注意：InkTag 的 OM6626 支持列表里没有 ZKC42V，只有 ZKC42B；所以 4.2 的面板/驱动需要实机确认，不能直接假设**
- 当前固件按 UC8276 / 400×300 BWR 写，但未实测

### EPD 引脚

两块板当前都按同一组引脚写：

```text
SCK   GPIO15  (SPI1_SCK)
MOSI  GPIO21  (SPI1_DO)
CS    GPIO22  (SPI1_CS)
DC    GPIO20  (GPIO)
RST   GPIO16  (GPIO)
BUSY  GPIO3   (GPIO input)
EN    GPIO7   (GPIO output, active high)
```

SPI1，3-wire，Mode 0，MSB first，8 MHz。

### RGB LED 引脚

```text
红  GPIO4   低电平点亮
绿  GPIO18  低电平点亮
蓝  GPIO17  低电平点亮
```

LED 状态：

- 蓝灯慢闪：广播中
- 绿灯常亮：已连接
- 青灯快闪：接收图片
- 红灯常亮：刷屏中
- 绿灯 1 秒：成功
- 红灯快闪：协议/CRC/传输错误

注意：GPIO4 也是 OM6626 普通 bootloader 文档里的 BOOT 脚。复位时应保持红灯灭（GPIO 高），不要在上电复位瞬间持续拉低红灯。

---

## 4. 代码位置

Linux/WSL（独立 git 仓库，master 为主分支）：

```text
<repo-parent>/freeink
    firmware/src        固件 C 源码（含 epd_ssd1680.c / epd_jd79668.c / led.c）
    firmware/gcc        Makefile（SDK_ROOT ?= ../../../OM6626-sdk）
    firmware/jlink      备份/烧录/恢复脚本
    host/zkc_epd.py     上位机
    tools/win/*.bat     Windows 一键脚本
    docs/protocol.md    docs/HANDOVER.md（本文）
```

Windows 发布目录：

```text
<Windows>\Downloads\freeink
```

主要文件：

```text
src/main.c              BLE + 主循环
src/board.c/h           SPI1、EPD 引脚、GPIO
src/led.c/h             RGB 状态灯
src/ble_epd.c/h         自定义 BLE GATT 服务、协议、CRC、图像缓冲
src/epd_ssd1680.c       2.1 寸 SSD1680 驱动（当前仍有花屏问题）
src/epd_uc8276.c        4.2 寸 UC8276 驱动（未实测）
src/epd.h               屏幕接口
docs/protocol.md        BLE 协议说明
host/zkc_epd.py         电脑端传图工具
host/requirements.txt   bleak + Pillow
build/                  编译输出
```

参考项目：

```text
<repo-parent>/ink-tag                InkTag 发行包（无固件源码，PolyForm Noncommercial）
<repo-parent>/OM6626-sdk             OnMicro OM6626 SDK（BSD 类许可）
<repo-parent>/EPD-nRF5               GPL-3.0 参考驱动，仅作资料，不要直接复制进闭源固件
```

---

## 5. 编译

工具链：

```text
<toolchain>/xpack-arm-none-eabi-gcc-12.3.1-1.2
```

建议用 ARM GNU 12.3.1；Debian 的 GCC 14 组合曾出现链接问题。

```sh
export PATH=<toolchain>/xpack-arm-none-eabi-gcc-12.3.1-1.2/bin:$PATH
cd <repo-parent>/freeink/firmware/gcc

make BOARD=ZKC21V -j4
make BOARD=ZKC42V -j4
```

输出在 `projects/zkc_epd/build/`：

```text
freeink_ZKC21V.bin   151,800 字节
freeink_ZKC21V.elf
freeink_ZKC42V.bin   151,576 字节
freeink_ZKC42V.elf
```

两个 bin 都链接在 `0x404000`，都小于第一级槽的 155,648 字节。

> 注意：2.1 的 151,800 字节只剩约 3.8KB 余量。后续加功能会超过第一级槽，需要改成 `0x404000` 小 bootloader + `0x42A000` app，或更新 MBR 分区长度。

---

## 6. 烧录 / 备份 / 恢复

### 当前 2.1 的布局

```text
0x400000-0x403FFF   OEM protected boot / CPFT / 工厂校准   不要碰
0x404000-0x429FFF   第一级 recovery 槽                       当前放自制 app
0x42A000-0x478000   InkTag 官方 app                         没有动，仍然在
0x480000-0x481FFF   InkTag 称为 runtime MBR / 保留区         没有动
0x4F0000-0x4F1FFF   当前固件的 NVDS                        备份里是空白
0x4F9000-0x4FCFFF   空白
0x4FD000-0x4FFFFF   InkTag NVDS / BKUP / CONF              没有动
```

自制 app 只覆盖 `0x404000` 起的第一级槽；`0x42A000` 的 InkTag 官方 app 还在。恢复时只要把原第一级 recovery 写回 `0x404000`，InkTag 就能重新启动。

### 备份文件

Linux/WSL：

```text
<repo-parent>/backup.mot
<repo-parent>/backup-0x404000.bin          1,032,192 字节，起始 0x404000
<repo-parent>/restore-appslot-0x404000.bin   155,648 字节，只含第一级槽
<repo-parent>/backup-README.txt
```

Windows：

```text
<Windows>\Downloads\host\backup\
```

`backup.mot` 已检查：

- Motorola S-record，32,256 条 S2 + S9
- 覆盖 `0x404000-0x4FFFFF`，连续无空洞
- 所有 S2 校验和正确
- `0x404000` recovery 与 InkTag 0.2.0 官方 `recovery.bin` SHA256 一致
- `0x42A000` app 与 InkTag 0.2.0 官方 OTA `application.bin` SHA256 一致

**注意：备份从 `0x404000` 开始，不包含 `0x400000-0x403FFF`。不要做全片擦除，否则这份备份恢复不了 protected boot/工厂校准。**

### 烧录

```text
device OM6626
si SWD
speed 4000
connect
halt
loadbin build\freeink_ZKC21V.bin, 0x404000
verifybin build\freeink_ZKC21V.bin, 0x404000
r
g
exit
```

不要全片擦除，不要写 `0x400000-0x403FFF`、`0x480000-0x481FFF`、`0x4F9000+`。

### 恢复

只恢复第一级槽：

```text
loadbin restore-appslot-0x404000.bin, 0x404000
verifybin restore-appslot-0x404000.bin, 0x404000
```

恢复整个已备份范围：

```text
loadbin backup-0x404000.bin, 0x404000
```

注意地址是 `0x404000`，不是 `0x400000`。

也可以重新运行 InkTag 官方完整烧录包（会清设备端设置，但保留 protected/MBR/工厂身份）：

```text
firmware/om6626_zkel/full/InkTag-om6626_zkel-full-v0.2.0.0-c4de6cfb54d7.zip
tools/configs/ZK_EL_OM6626_2.13_V1.1.toml
```

---

## 7. BLE 协议

自定义服务，不是 InkTag 协议：

```text
Service  6E400001-B5A3-F393-E0A9-E50E24DCCA9E
CMD      6E400002-...   write + notify
DATA     6E400003-...   write（offset + 数据块）
```

命令：

```text
0x01 INFO    返回宽高、格式、max_chunk
0x02 BEGIN   宽高、格式、总长度、CRC32
DATA          u32 offset + 图像数据
0x03 COMMIT  CRC32，触发刷屏
0x04 ABORT
0x05 CLEAR
```

图像格式 2：两平面 1bpp：

```text
黑平面：1 = 白，0 = 黑
红平面：1 = 无红，0 = 红
```

2.1 每平面 4000 字节，总 8000；4.2 每平面 15000，总 30000。

详细说明见 `docs/protocol.md`。

### BLE UUID 注意事项

最新源码使用标准 NUS UUID：

```text
6e400001-b5a3-f393-e0a9-e50e24dcca9e
```

之前刷过的一版固件曾把中间两组字节写反，实际暴露为：

```text
6e400001-a3b5-93f3-e0a9-e50e24dcca9e
```

`host/zkc_epd.py` 同时识别新旧两种 UUID，并会自动选择实际存在的服务。重新刷最新固件后应使用标准 UUID。

---

## 8. 电脑端工具

Windows：

```powershell
cd <Windows>\Downloads\host
python -m pip install -r requirements.txt

python zkc_epd.py --name ZKC21V --clear
python zkc_epd.py --name ZKC21V --test
python zkc_epd.py --name ZKC21V --image photo.jpg --rotate 90
```

常用参数：

```text
--name ZKC21V / ZKC42V
--address <BLE 地址>     多设备时指定
--test                   生成测试图案
--clear                  清屏
--image <文件>
--rotate 0/90/180/270
--mirror-x / --mirror-y
--preview out.png
--dither
--timeout 120
```

Windows 上已处理 GATT 缓存：`zkc_epd.py` 使用 `winrt={"use_cached_services": False}`；如果仍找不到服务，会打印当前 GATT 服务/特征值列表。

当前实测输出：

```text
Found ... ZKC21V
Connected: ... MTU=247
Device: 128x250 format=2 max_chunk=240
Uploading 8000 bytes...
Committed; waiting for panel refresh...
Panel refresh done.
```

传输和刷屏触发都成功，但最终显示仍然花屏。

---

## 9. 当前未解决问题：2.1 花屏

**现象：**

- BLE 连接、INFO、BEGIN、DATA、CRC、COMMIT 全部通过
- `--clear` 和 `--test` 都能触发屏幕刷新过程
- 刷新后屏幕是红/白/黑混杂的异常画面，不是预期的白屏或测试图案

**已排除：**

- 不是 BLE 连接问题
- 不是传输长度/CRC 问题
- 不是 COMMIT 未执行问题

**尚未确认：**

- 实际屏幕控制器是否确实是 SSD1680
- 屏幕 FPC 型号/批次是否与 InkTag 配置完全一致
- 当前 `epd_ssd1680.c` 的初始化、窗口、数据方向、刷新序列是否匹配这块具体屏
- BUSY 极性/等待时序是否正确
- 面板是否需要特定 LUT/VCOM/border 参数
- 图像数据的位序/平面顺序是否与控制器 RAM 约定一致

**建议下一步（按可靠性排序）：**

1. **确认屏幕实物型号**：拆开看 FPC 丝印，拍主板正反面和屏幕排线。不要只按“2.13 寸 BWR”判断。
2. **拿到该屏/该批次的厂商初始化序列**：SSD1680 只是控制器家族，不同玻璃/批次可能用不同 LUT、VCOM、border、update sequence。
3. **用逻辑分析仪抓一次原厂/InkTag 正常工作时的 SPI**：SCK/MOSI/CS/DC/RST/BUSY，记录上电初始化 + 一次清屏 + 一次图片刷新。这是最可靠的对照，不依赖逆向猜测。
4. **最小化验证**：先只做 `--clear`，目标是一整屏纯白；清屏都不正常时不要继续调图片数据。
5. **分开验证黑/红平面**：分别发送全黑、全红、全白，观察哪一层异常。
6. **核对 `0x11` 数据方向、`0x44/0x45` 窗口、`0x4E/0x4F` 计数器、`0x22` 刷新序列**，以实际屏 datasheet/厂商 demo 为准。
7. 如果 2.1 始终无法稳定，换一块已知 SSD1680 BWR 参考屏对比，或改用逻辑分析仪对照原厂固件。

**交接时请明确：当前 `epd_ssd1680.c` 是未验证的 bring-up 代码，不能当作已经调通的 SSD1680 驱动。**

---

## 10. 4.2 寸 ZKC42V（已调通）

当前代码：

```text
src/epd_jd79668.c     JD79668AA / BWR 双平面驱动
BOARD=ZKC42V          (board.h: 400x300, EPD_DEFAULT_FORMAT=2, BOARD_HAS_RGB_LED=0)
```

要点（均已实机验证）：

- 控制器是 **JD79668AA = SSD1680 兼容指令集**（不是 UC8276！用 UC 风格命令会把屏睡死）。
  InkTag 的 OM6626 4.2 配置 (ZK_EL_OM42BA_V1.0) 标的是 JD79668AA/BWRY 四色（那是 ZKC42B 的屏）；
  ZKC42V 这块实际焊的是 **BWR 三色** 面板，因此协议沿用 format 2（与 2.1 完全相同）。
- 序列：0x12 软复位+等BUSY → 0x01 (h-1,3字节) → 0x3C 0x01（边框！不是0x05）→ 0x18 0x80
  → 0x11 0x03（双向递增，与 2.1 的 0x01 不同）→ 0x44 00 31 / 0x45 0000_2B01 / 0x4E 0 / 0x4F 0
  → 0x22 0xB9 + 0x20 + 等BUSY；
  显示：0x24 黑平面(原生) / 0x26 红平面(取反) → 0x21 00 40 → 0x22 0xF7 + 0x20 + 等BUSY(30s)
  → 0x10 0x01 深睡。
- **方向：无需任何补偿**。屏原生横屏 400×300，`--clear`、`--test`、`--image` 不带
  rotate/mirror 参数即正确。host 里的 ZKC21V 默认补偿只对 (128,250) 生效，不影响 4.2。
- 板上无 LED：led.c 在 BOARD_HAS_RGB_LED=0 时所有函数编译为空操作。
- 备份状态注意：4.2 的出厂 flash 若尚未读取，第一次动手前先 savebin
  0x404000 起 0xFC000 字节（同 2.1 的流程），不要全片擦除。

---

## 11. 其他限制

- 没有 OTA；升级靠 SWD/J-Link。
- 没有低功耗/深睡；当前主循环常开。
- 没有局刷；每次都是全刷。
- 没有图片存 Flash/轮播；只有 RAM 中的一帧。
- 没有按键/NFC/电池电压/LED 策略配置。
- 没有 GUI；上位机是 Python CLI。
- 没有在 4.2 上测试过。
- 没有验证 OEM SecureBoot/EFUSE 状态；但 2.1 能启动自制 app，且没有动 protected/MBR/工厂区。**不要全片擦除。**

---

## 12. 许可

- InkTag 发行包/固件/CLI/WebUI：PolyForm Noncommercial 1.0.0；仓库不含固件源码。不要复制其代码/素材到商业产品。
- OnMicro OM6626 SDK：BSD 类许可，保留版权声明。
- EPD-nRF5：GPL-3.0；只作参考，不要把其代码直接复制进闭源固件。
- 本项目自有代码的许可证尚未最终确定，交接时需由项目所有者确认。

---



## 调试事故记录（2026-11-06，已结案）

主循环启用 PM 睡眠后，COMMIT/CLEAR 期间 BLE 触发的 light-sleep 切走了 SPI
时钟域，`drv_spi_transfer` 死等 TX FIFO，屏永远不刷。J-Link 现场抓取
PC=drv_spi_transfer+0x104 + golden 对照定位。修复：PM 改为 `make PM=1`
选配（默认关，行为与 golden 一致）；后续正解是刷屏期间
`pm_sleep_block()`。附带结论：EPD EN 必须上电即置高（运行时浪涌也会拖垮
总线）。Makefile 教训：`DEFINES +=` 必须写在 `DEFINES =` 之后，否则被整体
覆盖——A/B 实验必须 `cmp` 验证两组二进制确有不同。详见 docs/POWER.md。
## 13. 交接清单

需要一起交接的文件/目录：

```text
<repo-parent>/OM6626-sdk/projects/zkc_epd/
    src/
    host/
    docs/
    build/
    README.md
    HANDOVER.md

<repo-parent>/backup.mot
<repo-parent>/backup-0x404000.bin
<repo-parent>/restore-appslot-0x404000.bin
<repo-parent>/backup-README.txt

Windows:
<Windows>\Downloads\host\
    zkc_epd.py
    requirements.txt
    RUN-WINDOWS.txt
    HANDOVER.md
    build/
    backup/
```

接手后建议先做：

1. 读 `HANDOVER.md` 和 `README.md`；
2. 确认备份文件完整，不要全片擦除；
3. 用 `--clear` 复现当前花屏；
4. 拆 2.1 看屏幕 FPC/控制器；
5. 拿到正确的 SSD1680 初始化序列后再改 `epd_ssd1680.c`；
6. 2.1 显示正常后，再确认 4.2 面板型号，最后再开 4.2。
