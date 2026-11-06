#!/usr/bin/env python3
"""FreeInk e-ink calendar card generator (400x300 BWR, ZKC42V).

Renders a landscape desk-calendar for today (or --date):
  big day number, weekday, lunar date (农历), zodiac year,
  sexagenary day (干支) + twelve-jian almanac (黄历宜忌, 民俗参考),
  nearest solar term (节气), live weather from wttr.in.

Output is a 2x PNG (800x600) which zkc_epd.py downsamples + quantizes.

Usage:
  python calendar_card.py                     # today, Beijing weather
  python calendar_card.py --date 2026-11-06 --city Shanghai
  python calendar_card.py --no-weather -o card.png

Dependencies: Pillow, lunardate (pip). Network optional (wttr.in).
Fonts: tries Noto Sans CJK (~/.fonts), then Windows YaHei, then DejaVu.
"""

from __future__ import annotations

import argparse
import datetime as dt
import json
import math
import os
import sys
import urllib.request
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent / "_libs"))

from PIL import Image, ImageDraw, ImageFont

try:
    from lunardate import LunarDate
except ImportError:
    sys.exit("need 'pip install lunardate'")

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (200, 0, 0)

W, H, SCALE = 400, 300, 2
CW, CH = W * SCALE, H * SCALE

STEMS = "甲乙丙丁戊己庚辛壬癸"
BRANCHES = "子丑寅卯辰巳午未申酉戌亥"
ZODIAC = "鼠牛虎兔龙蛇马羊猴鸡狗猪"
CN_NUM = "零一二三四五六七八九十"
LUNAR_MONTHS = ["正", "二", "三", "四", "五", "六", "七", "八", "九", "十", "冬", "腊"]
JIAN12 = ["建", "除", "满", "平", "定", "执", "破", "危", "成", "收", "开", "闭"]
# folk almanac (十二值神宜忌, 简化民俗表, 仅供参考)
JIAN_YIJI = {
    "建": ("祭祀 会亲友", "出行 开业"),
    "除": ("除旧布新 沐浴", "嫁娶 搬家"),
    "满": ("栽种 订盟", "官司 安葬"),
    "平": ("修路 整手足甲", "诉讼 远行"),
    "定": ("嫁娶 签约 进人口", "词讼 出行"),
    "执": ("捕捉 缔约", "开业 嫁娶"),
    "破": ("治病 扫垣", "诸事不宜(大忌)"),
    "危": ("慎行 修置", "远行 动船"),
    "成": ("嫁娶 开业 移徙", "词讼"),
    "收": ("收割 纳财 祭祀", "出行 安葬"),
    "开": ("开业 入学 动土", "安葬 破土"),
    "闭": ("固封 求医", "开业 出巡"),
}
# 21st-century C constants of the 12 "节" (day = int(y*0.2422 + C) - L)
JIE = [  # (gregorian month, C, month-branch index 子0..亥11)
    (2, 3.875, 2),    # 立春 -> 寅
    (3, 5.63, 3),     # 惊蛰 -> 卯
    (4, 4.81, 4),     # 清明 -> 辰
    (5, 5.52, 5),     # 立夏 -> 巳
    (6, 5.678, 6),    # 芒种 -> 午
    (7, 7.928, 7),    # 小暑 -> 未
    (8, 7.5, 8),      # 立秋 -> 申
    (9, 7.646, 9),    # 白露 -> 酉
    (10, 8.43, 10),   # 寒露 -> 戌
    (11, 7.438, 11),  # 立冬 -> 亥
    (12, 7.18, 0),    # 大雪 -> 子
    (1, 5.4055, 1),   # 小寒 -> 丑
]
JIE_NAMES = {2: "立春", 3: "惊蛰", 4: "清明", 5: "立夏", 6: "芒种", 7: "小暑",
             8: "立秋", 9: "白露", 10: "寒露", 11: "立冬", 12: "大雪", 1: "小寒"}
WEEKDAYS = "一二三四五六日"


def font_pair(size):
    """Resolve (regular, bold) CJK fonts on WSL/Linux/Windows."""
    regulars = [
        os.path.expanduser("~/.fonts/NotoSansCJKsc-Regular.otf"),
        "C:/Windows/Fonts/msyh.ttc",
        "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
        "/usr/share/fonts/truetype/wqy/wqy-zenhei.ttc",
    ]
    bolds = [
        os.path.expanduser("~/.fonts/NotoSansCJKsc-Bold.otf"),
        "C:/Windows/Fonts/msyhbd.ttc",
        "/usr/share/fonts/opentype/noto/NotoSansCJK-Bold.ttc",
    ]

    def pick(paths):
        for p in paths:
            if os.path.exists(p):
                try:
                    return ImageFont.truetype(p, size * SCALE)
                except Exception:
                    pass
        return None

    reg = pick(regulars) or ImageFont.load_default()
    bold = pick(bolds) or reg
    return {False: reg, True: bold}


def cn_day_of_month(d):
    if d == 10:
        return "初十"
    if d < 10:
        return "初" + CN_NUM[d]
    if d < 20:
        return "十" + CN_NUM[d - 10]
    if d == 20:
        return "二十"
    if d < 30:
        return "廿" + CN_NUM[d - 20]
    return "三十"


def ganzhi_day(date):
    # day pillar index = (JDN + 49) % 60, JDN = date.toordinal() + 1721425
    idx = (date.toordinal() + 1721474) % 60
    return idx, STEMS[idx % 10], BRANCHES[idx % 12]


def jie_day(year, month, c):
    y = year % 100
    l = (y if month <= 2 else y - 1) // 4
    return int(y * 0.2422 + c) - l


def month_branch(date):
    """current month's earthly branch via the 节 boundaries."""
    best = None
    for mm, c, br in JIE:
        for yy in (date.year - 1, date.year):
            try:
                d = dt.date(yy, mm, jie_day(yy, mm, c))
            except (ValueError, OverflowError):
                continue
            if d <= date and (best is None or d > best[0]):
                best = (d, br)
    return best[1]


def next_jie(date):
    """nearest solar term on/after `date` (the 12 '节')."""
    cands = []
    for mm, c, br in JIE:
        for yy in (date.year, date.year + 1):
            try:
                d = dt.date(yy, mm, jie_day(yy, mm, c))
            except (ValueError, OverflowError):
                continue
            if d >= date:
                cands.append((d, JIE_NAMES[mm]))
    return min(cands)[::-1] if cands else (None, None)


# 常见城市英文名 -> 中文
CITY_CN = {"Beijing": "北京", "Shanghai": "上海", "Shenzhen": "深圳",
           "Guangzhou": "广州", "Hangzhou": "杭州", "Nanjing": "南京",
           "Chengdu": "成都", "Wuhan": "武汉", "Xi'an": "西安", "Chongqing": "重庆"}

# wttr.in / OpenWeather weather code -> 中文
WX_CODE_CN = {
    "113": "晴", "116": "多云", "119": "阴", "122": "阴",
    "143": "雾", "176": "有雨", "200": "雷阵雨", "248": "雾", "260": "冻雨",
    "266": "小雨", "296": "雨", "299": "中雨", "302": "大雨", "305": "小雨",
    "308": "暴雨", "311": "冻雨", "314": "大雨", "317": "雨夹雪", "320": "雪",
    "323": "阵雪", "326": "小雪", "329": "中雪", "332": "大雪", "335": "大雪",
    "338": "雪", "350": "冻雪", "353": "阵雨", "356": "强阵雨", "359": "强阵雨",
    "386": "雷阵雨", "389": "强雷阵雨", "392": "强雷阵雨", "395": "暴雪",
}


def get_weather(city, timeout=8):
    url = f"https://wttr.in/{urllib.request.quote(city)}?format=j1"
    try:
        with urllib.request.urlopen(url, timeout=timeout) as r:
            d = json.load(r)
        cur = d["current_condition"][0]
        today = d["weather"][0]
        code = today["hourly"][4].get("weatherCode", "")
        desc = WX_CODE_CN.get(code)
        if not desc:
            zh = cur.get("lang_zh")
            desc = zh[0]["value"] if zh else cur["weatherDesc"][0]["value"]
        return {
            "city": CITY_CN.get(city, city), "now": cur["temp_C"] + "℃",
            "desc": desc,
            "lo": today["mintempC"], "hi": today["maxtempC"],
            "hum": cur.get("humidity", "--"),
        }
    except Exception:
        return None


def draw():
    ap = argparse.ArgumentParser()
    ap.add_argument("--date", default=None)
    ap.add_argument("--city", default="Beijing")
    ap.add_argument("--no-weather", action="store_true")
    ap.add_argument("-o", "--out", default=None)
    a = ap.parse_args()

    date = (dt.date.fromisoformat(a.date) if a.date else dt.date.today())
    img = Image.new("RGB", (CW, CH), WHITE)
    d = ImageDraw.Draw(img)

    fbig = font_pair(86)[True]
    f18, f18_b = font_pair(18)[False], font_pair(18)[True]
    f14, f14_b = font_pair(14)[False], font_pair(14)[True]
    f12 = font_pair(12)[False]
    f11 = font_pair(11)[False]

    def T(x, y, s, font, fill=BLACK, anchor="la"):
        d.text((x * SCALE, y * SCALE), s, font=font, fill=fill, anchor=anchor)

    def R(x, y, w, h_, fill):
        d.rectangle((x * SCALE, y * SCALE, w * SCALE, h_ * SCALE), fill=fill)

    # ---- top red band ----
    R(0, 0, W, 40, RED)
    T(12, 20, f"{date.year}年{date.month}月", f18_b, WHITE, anchor="lm")
    wd = WEEKDAYS[date.weekday()]
    T(W - 12, 20, f"星期{wd}", f18_b, WHITE, anchor="rm")
    weekend = date.weekday() >= 5

    # ---- left: big day number ----
    T(88, 105, str(date.day), fbig, RED if weekend else BLACK, anchor="mm")

    # ---- left under number: lunar + day pillar ----
    ld = LunarDate.from_solar_date(date.year, date.month, date.day)
    lunar_s = f"农历{LUNAR_MONTHS[ld.month - 1]}月{cn_day_of_month(ld.day)}"
    year_gz = STEMS[(ld.year - 4) % 10] + BRANCHES[(ld.year - 4) % 12]
    zodiac = ZODIAC[(ld.year - 4) % 12]
    gi, gs, gb = ganzhi_day(date)
    mb = month_branch(date)
    star = JIAN12[((gi % 12) - mb) % 12]
    yi, ji = JIAN_YIJI[star]
    lx = 14
    T(lx, 176, lunar_s, f18, BLACK)
    T(lx, 200, f"{year_gz}年 · 属{zodiac}", f14, BLACK)
    T(lx, 220, f"{gs}{gb}日 · {star}日", f12, BLACK)

    # ---- vertical divider ----
    R(168, 52, 169, 252, BLACK)

    # ---- right: weather ----
    rx = 182
    T(rx, 62, "天气", f14_b, RED)
    wx = None if a.no_weather else get_weather(a.city)
    if wx:
        T(rx, 82, f"{wx['city']} · {wx['desc']}", f14, BLACK)
        T(rx, 101, f"{wx['now']}  今 {wx['lo']}~{wx['hi']}℃", f12, BLACK)
        T(rx, 118, f"湿度 {wx['hum']}%", f12, BLACK)
    else:
        T(rx, 82, "--", f12, BLACK)

    # ---- right: solar term ----
    nj, nd = next_jie(date)
    if nj:
        delta = (nd - date).days
        rel = "今日" if delta == 0 else "明日" if delta == 1 else ""
        T(rx, 140, f"{rel}{nj}" if rel else f"{nj} · {delta}天后", f14_b, RED)

    # ---- right: almanac yi/ji with red chips ----
    def chip(x, y, ch):
        R(x, y, x + 16, y + 16, RED)
        T(x + 8, y + 8, ch, f12, WHITE, anchor="mm")

    chip(rx, 166, "宜")
    T(rx + 22, 174, yi, f12, BLACK, anchor="lm")
    chip(rx, 188, "忌")
    T(rx + 22, 196, ji, f12, BLACK, anchor="lm")

    # ---- bottom hairline + footer ----
    R(12, 252, W - 12, 253, BLACK)
    T(lx, 268, f"第{date.isocalendar()[1]}周 · 今年第{date.timetuple().tm_yday}天", f11, BLACK)
    T(W - 12, 268, "FreeInk", f12, RED, anchor="ra")

    out = a.out or f"examples/calendar_{date.isoformat()}.png"
    img = img.resize((W, H), Image.Resampling.LANCZOS)
    Path(out).parent.mkdir(exist_ok=True)
    img.save(out)
    print(out)


if __name__ == "__main__":
    draw()
