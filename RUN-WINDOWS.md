FreeInk - Windows 快速上手
================================

目录长这样（本文件夹就是 FreeInk 仓库根）：

    host\zkc_epd.py        上位机（发图/测试/清屏）
    host\examples\         draw.io 图卡模板
    build\                 固件 bin（freeink_ZKC21V.bin / freeink_ZKC42V.bin）
    jlink\                 J-Link Commander 脚本
    tools\win\*.bat        一键脚本（本文件有说明）
    backup\                出厂固件备份（重要！别删）


1. 装 Python 依赖（只需一次）
--------------------------------
双击  tools\win\setup.bat
（要求 Python 3.10+，安装时勾选 "Add to PATH"）


2. 发图上屏
--------------------------------
双击运行 cmd（或直接双击 bat，会闪退就用 cmd）：

    cd <Windows>\Downloads\freeink\tools\win
    test.bat 21            给 2.1 发内置测试卡
    send.bat 21 C:\path\photo.jpg            发任意图片
    send.bat 42 C:\path\card.png --preview out.png   先看转换结果
    clear.bat 21 | 42      清屏

等价的手敲命令（PowerShell）：

    cd <Windows>\Downloads\freeink
    py -3 host\zkc_epd.py --name ZKC21V --image mypic.png
    py -3 host\zkc_epd.py --name ZKC42V --image mypic.png

说明：
- ZKC21V 的方向补偿已内置（rotate90+mirror-y），横版图直接发。
- ZKC42V 原生横屏，什么都不用带。
- 同一张桌上两块板都在时，用 --address 指定，或先关掉另一块。
- 常用参数：--rotate 0/90/180/270  --mirror-x/--mirror-y
            --threshold 128  --red-threshold 110  --dither  --preview out.png
- 刷屏期间面板会黑白红闪几次，属正常；COMMIT 后最长约 30 秒完成。


3. 重新烧固件（改过 firmware 之后）
--------------------------------
1) WSL 里构建：
       cd ~/ep/freeink/firmware/gcc
       export PATH=<toolchain>/xpack-arm-none-eabi-gcc-12.3.1-1.2/bin:$PATH
       make BOARD=ZKC21V -j4 && make BOARD=ZKC42V -j4
   产物: ~/ep/freeink/firmware/build/freeink_ZKCxx.bin
   拷到本文件夹 build\ 下（同名覆盖）。
2) 接好 J-Link + 排针，双击：
       tools\win\flash_21v.bat    刷 2.1
       tools\win\flash_42v.bat    刷 4.2

铁律：
- 不要全片擦除！只写 0x404000。
- 新板子第一次动手前，先跑 tools\win\backup_42v.bat 存一份出厂固件。


4. 反悔恢复
--------------------------------
       tools\win\restore_21v.bat    2.1 恢复 InkTag 原厂
       tools\win\restore_42v.bat    4.2 从 backup\zkc42v-app-0x404000.bin 恢复
（restore_42v 需要你先做过 backup_42v）


5. 故障速查
--------------------------------
扫描不到设备        板子没电/被别的手机连着；换 --address 直连
找不到 GATT 服务    固件太旧，重烧最新 bin
--preview 正常但屏花  等 BUSY 逻辑问题或序列不对：见 docs/HANDOVER.md
文字镜像/方向不对    先 --preview 确认数据没错，再试 --rotate/--mirror 组合
发送特别慢          正常：30000 字节 × 240 分块 + 每条写响应
