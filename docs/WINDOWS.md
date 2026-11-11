# Windows 使用指南

板子已按 [HARDWARE.md](HARDWARE.md) 备份并烧好固件之后，日常发图、清屏、烧录、恢复都在这份文档里。
协议细节见 [protocol.md](protocol.md)，参数全表也可以 `python host\zkc_epd.py --help` 现查。

## 1. 目录地图

发布目录 `<下载>\freeink\` 本身就是仓库根：

```text
host\zkc_epd.py        上位机（发图/测试/清屏）
host\examples\         图卡模板与成品（draw.io 卡片、日历示例）
build\                 固件 bin（freeink_ZKC21V.bin / freeink_ZKC42V.bin）
firmware\jlink\        J-Link Commander 脚本
tools\win\             一键 .bat（下表）
backup\                出厂固件备份——重要，别删
```

## 2. 环境准备（一次）

推荐 uv：

```text
uv venv
uv pip install -r host\requirements.txt
```

之后手敲命令用 `.venv\Scripts\python host\zkc_epd.py …` 或 `uv run host\zkc_epd.py …`。
一键 bat 内部调 `py -3`，依赖装在系统 Python 里时开箱即用。

## 3. 一键脚本

双击 bat 闪退的话，开 cmd 在 `tools\win` 里跑同一条命令——至少能看到报错。

| 命令 | 作用 |
|---|---|
| `test.bat 21` / `test.bat 42` | 发内置测试卡（角标 + 坐标轴，直接检验旋转/镜像/方向） |
| `send.bat 21 C:\path\photo.jpg` | 发任意图片，后面可追加 host 参数 |
| `send.bat 42 C:\path\card.png --preview out.png` | 先看量化预览，再决定上屏 |
| `clear.bat 21` / `clear.bat 42` | 清屏，应整屏纯白 |
| `daily.bat 21` / `daily.bat 42` | 生成当日卡片并上屏：21=竖排日报卡，42=整月日历（农历、干支、宜忌、节气、天气） |
| `backup_21v.bat` / `backup_42v.bat` | 出厂备份 → `backup\`，新板第一次动手前必跑 |
| `flash_21v.bat` / `flash_42v.bat` | 烧固件到 0x404000 |
| `flash_21v_ab.bat` | 烧 boot v0 + slot A 双槽布局 |
| `flash_21v_pmtest.bat` / `flash_42v_pmtest.bat` | 烧 PM 候选构建（台架验证用） |
| `restore_21v.bat` / `restore_42v.bat` | 恢复出厂固件（`restore_42v` 需先做过 `backup_42v`） |
| `probe_21v.bat` | 板子卡死时抓现场（PC/LR），分析见 [BRINGUP.md](BRINGUP.md) 第 5 节 |
| `find_jlink.bat` | 找 J-Link：PATH → Program Files → 手输一次，路径记进 `jlink.cfg` |

## 4. 等价手敲命令

```powershell
cd <下载>\freeink
py -3 host\zkc_epd.py --name ZKC21V --image mypic.png
py -3 host\zkc_epd.py --name ZKC42V --image mypic.png
py -3 host\zkc_epd.py --name ZKC21V --clear
```

## 5. 参数

| 参数 | 说明 | 默认 |
|---|---|---|
| `--name` | BLE 广播名前缀 | 自动识别 ZKC21V/ZKC42V |
| `--address` | 直连地址，跳过扫描 | — |
| `--image` | 输入图片（PNG/JPEG 等 Pillow 可读格式） | — |
| `--test` / `--clear` | 内置测试卡 / 清屏 | — |
| `--rotate {0,90,180,270}` | 旋转 | ZKC21V 自动补偿 90 |
| `--mirror-x` / `--mirror-y` | 镜像 | 关 |
| `--fit {crop,stretch}` | 等比裁剪填充 / 直接拉伸 | crop |
| `--threshold` | 黑白亮度阈值 | 128 |
| `--red-threshold` | 红色判定阈值 | 110 |
| `--dither` | Bayer 有序抖动，照片类内容建议开 | 关 |
| `--preview PATH` | 输出量化预览 PNG | — |
| `--scan-timeout` | 扫描超时（秒） | 8 |
| `--timeout` | 刷屏等待（秒） | 120 |

## 6. 行为注记

- ZKC21V 的方向补偿已内置（rotate 90 + mirror-y），横版图直接发；ZKC42V 原生横屏，什么都不用带。
- LED 默认策略：广播时全灭，连接/传图/刷屏才亮。想要蓝灯心跳，把 `board.h` 的
  `LED_IDLE_BLINK` 改回 1 重编固件。
- 同桌两块板都在广播时，用 `--address` 点名，或先关掉另一块。
- 刷屏期间面板黑白红闪几次属于正常工作过程；COMMIT 后最长约 30 s 完成。
- 发送慢不是故障：4.2 一张 30,000 字节按 240 字节分块、逐条等待写响应，就是需要这么久。
- UUID 版本：当前固件用标准 NUS UUID（`6e400001-b5a3-f393-…`）；更早的一版固件曾把中间两段
  字节写反（`…-a3b5-93f3-…`）。host 两种都识别、自动挑实际存在的那个；重烧最新固件后按标准走。
- Windows GATT 缓存：host 已带 `winrt={"use_cached_services": False}`；仍找不到服务时，
  host 会打印当前可见的 GATT 服务/特征值列表。

## 7. 故障速查

| 症状 | 处置 |
|---|---|
| 扫描不到设备 | 板子没电，或已被别的中心设备（常见：某台手机）连着。换 `--address` 直连，或断开占用方 |
| 找不到 GATT 服务 | 板上是旧版固件（UUID 反转那版），重烧最新 bin |
| `--preview` 正常但屏花 | 驱动时序问题，按 [BRINGUP.md](BRINGUP.md) 第 4 节排查 |
| 文字镜像/方向不对 | 先看 `--preview` 确认数据本身没错，再组合 `--rotate` / `--mirror` |
| 发送特别慢 | 正常现象，见第 6 节 |
| 屏完全不动、BLE 假死 | `probe_21v.bat` 抓 PC，按 [BRINGUP.md](BRINGUP.md) 第 5 节分析 |
