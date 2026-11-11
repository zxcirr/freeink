# OTA 设计：A/B 双槽与可信启动

目标：不用 J-Link 就能更新固件，并且把"刷坏"的后果限制在 boot 层——只要坏的不是 boot 本身，
设备都能自己站起来。

回答两个前置问题，然后给出完整方案：

- **flash 和 CPU 够不够做 A/B？** 够，不需要退到单 boot + 单 app。账目见第 1 节。
- **app 出错怎么办？** 两道防线：trial/confirm 回退（app 起不来就回上一个好固件，设备照常工作）；
  boot 兜底 rescue 模式（两槽全坏时 boot 常开蓝牙 DFU，等重新灌入）。

## 1. 资源盘点

### 1.1 flash：1 MiB，A/B 绰绰有余

总容量 1 MiB（0x400000–0x4FFFFF）。旁证：SDK 的 MBR 组件把配置区按 `capacity − 4×4 KB` 贴着
flash 顶部排布，而出厂 NVDS/BKUP/CONF 实测位于 0x4FD000–0x4FFFFF——顶部就是 0x500000。
精确值可用 `drv_sfs_capacity()` 运行时读一次，列入 M0 验证项。

拟定分区表（在 boot v0 布局上加一个元数据页，其余不动）：

| 地址 | 内容 | 大小 | 边界 |
|---|---|---|---|
| 0x400000–0x403FFF | OEM protected boot / CPFT / 工厂校准 | 16 KB | 不碰。SDK flash 驱动对内部 flash 第 4 扇区（0x403000 起）有硬保护，与这条边界吻合 |
| 0x404000–0x413FFF | FreeInk boot v1 | 64 KB | boot 现有 bin 45,856 B，加 DFU 胶水后仍在预算内（见 5.3） |
| 0x414000–0x443FFF | app slot A | 192 KB | 当前 app 151,832 B，余 40 KB |
| 0x444000–0x473FFF | app slot B | 192 KB | 同上 |
| 0x474000–0x47FFFF | 槽扩展余量 | 48 KB | slot 最长可扩到 240 KB，仍不碰 0x480000 |
| 0x480000–0x481FFF | 出厂保留区 | 8 KB | 不碰 |
| 0x482000–0x4EFFFF | 自由区 | ~276 KB | 留给图片存储、日志等 |
| 0x4F0000–0x4F1FFF | NVDS（当前固件在用） | 8 KB | 不动 |
| 0x4F2000–0x4F2FFF | OTA 元数据页 | 4 KB | 本设计新增 |
| 0x4F3000–0x4F8FFF | 留白 | 24 KB | — |
| 0x4F9000–0x4FFFFF | 出厂 NVDS / BKUP / CONF | 28 KB | 不碰 |

### 1.2 单槽 + staging 为什么不需要

| | A/B 双槽 | 单槽 + staging（写暂存区，重启时搬移） |
|---|---|---|
| flash 占用 | 2 × app 槽 | app 槽 + 暂存区（省约 192 KB） |
| 断电安全性 | 任意时刻断电都不破坏现有固件（写入只发生在非运行槽） | 搬移阶段需要额外保护算法（扇区轮换，类似 MCUboot swap） |
| 回滚成本 | 零（boot 换个跳转目标） | 需要把搬移过程做成可恢复事务 |
| boot 复杂度 | 校验 + 跳转 | 校验 + 搬移状态机 |
| 本项目成立条件 | app ≤ 240 KB（还可扩） | app 槽省出的空间放图片存储更划算时才值得 |

本项目 app 152 KB、槽 192 KB、自由区还有 ~276 KB——A/B 方案的"缺点"（占两份空间）在这里
不构成代价，而它的全部优点（简单、断电安全、即时回滚）都是我们要的。**决策：A/B。**
单槽 + staging 保留为"app 长过 240 KB"时的备选路线。

### 1.3 CPU / RAM

- BLE 协议栈在 ROM 库里（这也是 app bin 只有 ~150 KB 的原因）。boot 或 app 加 DFU，成本只是
  我们自己的胶水代码（GATT 处理 + CRC + flash 驱动调用），不是整栈。
- OTA 写入走 `drv_sf_write`/`drv_sf_erase`（4 KB 扇区）。SDK 初始化时默认开启
  `drv_sf_auto_suspend_enable(true)`：擦写进行中会 suspend 扇区擦除、放行 XIP 取指，擦完 resume。
  所以"app 一边跑一边往另一个槽写"不会卡死取指——这条要上台架确认（见 M2 验证项），
  但机制在驱动层已经具备。
- RAM 80 KB（链接脚本 `__RAM_SIZE = 0x13EA0`）。分块写入只需要 240 B 缓冲 + 协议状态，
  与现有图片传输同量级，无压力。
- PM 注意：将来启用 `PM=1` 后，DFU 期间同样要 `pm_sleep_prevent` 包住（PM 事故的直接推论，
  见 BRINGUP.md 第 5 节）。默认构建不睡系统，无此问题。

## 2. 启动决策（boot v1）

```text
OEM protected boot (0x400000)
        │
FreeInk boot v1 (0x404000)
        │ 读 OTA 元数据页
        ├─ 有 trial 槽且 counter > 0 ？
        │      └─ 校验（向量表 + CRC）→ 通过：跳该槽；app 自检后调用 dfu_confirm()
        │                                  失败：counter-- 写回元数据，落入下一分支
        ├─ confirmed 槽校验通过 → 跳转
        ├─ confirmed 无效，另一槽校验通过 → 写元数据（confirmed = 该槽）→ 跳转
        └─ 全部无效 → rescue 模式：BLE 常开 DFU，红灯慢闪，无限等待
```

### 2.1 trial / confirm（防砖核心）

一次合法的 OTA 永远不会消灭手上的好固件：

1. 新镜像写入**非运行槽**，CRC 通过后在元数据里标 `trial`（counter = 3），保持旧槽 `confirmed`；
2. 重启后 boot 优先试 trial 槽；
3. 新 app 自检通过（BLE 服务起来 + EPD 初始化完成）即调用 `dfu_confirm()`，元数据把
   `confirmed` 挪到新槽；
4. 新 app 若每次都没能 confirm（崩了、死循环了），boot 每次启动 `counter--`，归零后自动回退
   confirmed 旧槽。

counter 递减由 boot 在跳转前写入元数据（必须落盘，否则复位计数器形同虚设）。
host 也能通过 DFU_CONFIRM（opcode 0x15）手动确认——适用于"app 其实正常，只是自检没跑到"的场景。

### 2.2 元数据页格式（0x4F2000）

4 KB 页分两半（各 2 KB），轮流使用；同一时刻整页只有一份记录集是新鲜的：

```text
记录集 {
  magic   u32  'FOIM'
  seq     u32  单调递增
  recA    { size u32, crc32 u32, flags u8(confirmed|trial), counter u8 }
  recB    同构
  crc32   u32  对 recA + recB
}
```

更新流程：擦"另一半" → 写新集（seq+1）→ 回读校验。旧半页在擦除前始终完好，
任意时刻断电都至少有一份可信记录。boot 读：两半都验，取 magic + CRC 通过且 seq 大者；
两半都坏视为"无元数据"（首次使用或整页损坏），此时按 CRC 全凭槽内自校验走 rescue。

槽内镜像本身保持现状（向量表在槽首，无头部）——boot 跳转逻辑不用改，VTOR 直指槽基址。
size/CRC/版本全部记账在元数据页。

### 2.3 boot v0 的现存 bug（M0 必修）

`firmware/boot/src/boot.c` 里 `RAM_HIGH` 少写了一个零：`0x20001400`（5 KB），应为
`0x20014000`（80 KB，与链接脚本 `__RAM_SIZE=0x13EA0` 一致）。slot A 镜像的初始 SP 是
`0x20005180`——按现状判断会被当成非法向量表拒跳，然后 slot B 同理，最后直接掉进红灯呼救。
也就是说 **A/B 链路当前根本跑不通**，这不是 OTA 的新问题，是 v0 遗留。修法：直接用链接脚本
导出的符号，别再手写魔数。

## 3. OTA 传输协议（app 运行时）

复用现有 GATT 服务与 CMD 特征（NUS 风格，MTU 247），在 0x01–0x05 之后追加：

| opcode | 命令 | 载荷 | 行为 |
|---|---|---|---|
| 0x10 | DFU_BEGIN | target u8（0=自动取非运行槽，1=A，2=B）、size u32、crc32 u32 | 预登记目标槽，回复 max_chunk |
| 0x11 | DFU_DATA | offset u32 + ≤240 B | 惰性擦：offset 跨入新 4 KB 扇区时先擦后写；逐块回 ACK，节奏与图片传输一致 |
| 0x12 | DFU_END | — | 对已写区间做 CRC32；通过 → 元数据记 trial → 回 ACK → 1 s 后自复位 |
| 0x13 | DFU_ABORT | — | 放弃本次会话，目标槽保持脏（下次 BEGIN 重新擦） |
| 0x14 | DFU_STATUS | — | 运行槽 / confirmed 槽 / trial 状态 / counter / 版本 |
| 0x15 | DFU_CONFIRM | — | 立即 confirm 当前运行槽（app 自检走同一入口） |

状态码沿用 0x00–0x05（OK / INVALID / BUSY / CRC / SIZE / STATE），协议规范补进
[protocol.md](protocol.md)。

设计要点：

- **只在非运行槽写**。执行中的代码永远不写自己脚下的扇区——这是 A/B 方案"断电任意时刻安全"
  的前提，一条纪律换掉一整类踩坑。
- **BEGIN 不整槽擦**。192 KB = 48 扇区连擦约数秒，足以掐断连接事件。按 offset 惰性擦把成本
  摊到整个传输里，每次几十毫秒，BLE 无感。
- **中途断电无害**。半截镜像 CRC 过不了，boot 照常选 confirmed 旧槽；重试时 DFU_BEGIN 重新擦。
- **速率账**。192 KB ≈ 819 块 × 240 B，按现有图片传输的节奏大约几分钟。第一版不做压缩、
  不做差分；LZMA 全量（省 60–80% 传输时间）与差分升级列为 v2 选项。
- **鉴权（开放问题）**。现有协议无任何鉴权，rescue 模式更是完全裸奔。价签场景风险低，但如果
  要防"邻居顺手刷机"，可在 NVDS 存 4 字节配对码，DFU_BEGIN 校验。先不做，记入开放问题。

## 4. rescue 模式（boot 常开蓝牙 DFU）

触发条件只有一个：两槽全部不可信（首次空板、或两次更新都失败）。只要存在一条 confirmed 槽，
永远轮不到它。

- boot 自带 BLE 外设：boot 的 Makefile 本来就链接 `CONFIG_LIB_PRESET_BLE_1PERIPHERAL`，
  栈在 ROM，加的是 GATT 胶水。只开 DFU 最小命令集（0x10/0x11/0x12/0x13）。
- 广播名加后缀区分身份（如 `ZKC21V-BL`），host 一眼知道对面是 boot 不是 app；红灯慢闪。
- 写入目标约定为 slot A，写完标 trial 复位。
- boot 不做刷屏、不做业务 NVDS、不做 PM——rescue 是维护状态，越简单越不会自己坏。

### 4.1 boot 体积账

| | 大小 |
|---|---|
| 预算 | 64 KB |
| 现 bin（含 shell/trace） | 45,856 B |
| 余量 | 19.7 KB |
| 预计 DFU 胶水增量 | 10–20 KB |

偏紧。缓解顺序：先把 boot 构建里的 shell/trace 裁掉（调试期便利，量产是负资产，可省数 KB）；
仍不够则把 boot 区扩到 72 KB、slot A 缩到 184 KB（仍 > 151,832 B）。不动"砍 DFU"这条底线。

## 5. app 侧行为

### 5.1 现在（蓝牙常开，即第一版）

- 广播策略不变：常开。
- 新增一处：自检通过（BLE 服务 up + EPD 初始化完成）后调用 `dfu_confirm()`。就这一行，
  防砖语义全靠它收口。

### 5.2 之后（NFC 唤醒 10 分钟窗口）

广播策略做成状态机，OTA 逻辑不感知：

```text
QUIET ──NFC IRQ──▶ ADVERTISING(10 min) ──超时──▶ QUIET
                        │
                     建立连接 / 传输中 → 窗口自动续期，连接不断
```

- QUIET 模式下广播关、CPU 深睡——依赖 PM 正解（刷屏与传输期间 `pm_sleep_prevent`），
  也就是 PM 事故里已铺好方向的那条路。
- 唤醒源：NFC 芯片的 field-detect 中断接 GPIO。**板上有没有 NFC 芯片还没勘察过**——M4 之前
  先看主板：有无 NFC 天线走线、IC 丝印（ZKC 价签常见 NTAG/ST25DV I²C 系列）。有芯片接
  IRQ 即可；没有则换按键或定时方案。
- 10 分钟窗口时长、设备名等配置存 NVDS，host 可改。

### 5.3 DFU 期间的 LED

青灯快闪 = DFU 传输（复用现有"接收图片"灯语），成功/失败沿用绿灯 1 s / 红灯快闪。
rescue 模式专属红灯慢闪，与 app 侧错误灯（快闪）区分开。

## 6. 里程碑

每步都有独立的验证标准，不过机器验证不算完成：

| | 内容 | 验证标准 |
|---|---|---|
| M0 | 修 boot.c RAM_HIGH typo；`drv_sfs_capacity()` 读数确认 flash 容量 | 实机：boot → slot A 链路跑通（当前就被此 bug 挡住）；再单独刷 slot B，确认选择逻辑两个方向都对 |
| M1 | 元数据页读写 + boot v1 选择逻辑（trial/confirm/回退）；app 加 `dfu_confirm()` | 实机：OTA 模拟——B 槽故意烧坏镜像 → boot 回退 A；B 槽好镜像 3 次不 confirm → 回退；confirm 后重启直接进 B |
| M2 | app 内 DFU 传输（写对侧槽） | 台架：192 KB 连续擦写期间 BLE 心跳不断连（验证 auto-suspend）；传输中途拔电 → 旧固件照常启动；坏包 CRC → 拒绝 commit |
| M3 | boot rescue DFU | 实机：双槽清空 → boot 广播 `*-BL` → 从 host 灌入固件 → 恢复运行 |
| M4 | NFC 勘察 + QUIET/10 min 窗口（可选，独立于 OTA） | 板上确认 NFC 芯片；PM 正解落地后，QUIET 空闲电流 < 1 mA（POWER 预期值） |

M0–M1 是纯 boot/元数据工程，不碰 BLE 协议；M2 才引入传输。每步都可单独发布，
中途任何一步停住，设备仍是"能用的电子价签"。

## 7. 风险清单

| 风险 | 影响 | 处置 |
|---|---|---|
| OEM SecureBoot/EFUSE 状态未知 | 若启用签名校验，boot 链路需要 OEM 认证 | 现状 2.1/4.2 均已实测放行自制镜像（从 0x404000 启动过），风险低；保持记录，M0 顺带复核 |
| 擦写期间 XIP 取指 | BLE 断连、总线卡死 | 驱动 auto-suspend 机制已具备；M2 台架专门验证，不通过则在擦除间隙让步（每擦 1 扇区让出若干 ms） |
| boot 太大塞不进 64 KB | rescue 模式装不下 | 裁 shell/trace → 扩 boot 区到 72 KB 的预案顺序见 4.1 |
| 元数据页损坏 | 两半都坏 → 回到 rescue | 双半页 + seq 仲裁已把概率压到"同时坏两半"；真发生也就是走一次 rescue |
| flash 寿命 | 每次 OTA 擦 48 扇区 + 元数据 1 页 | 每扇区 10 万次擦写，按一天一次 OTA 也够用几年；元数据双半页轮替再加一倍寿命 |
| app 崩在 confirm 之前但 BLE 还活着 | 3 次启动后被回退 | DFU_STATUS 可查 counter；host 可 DFU_CONFIRM 手动救——语义明确，不是事故 |
