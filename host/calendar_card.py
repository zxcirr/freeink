#!/usr/bin/env python3
"""FreeInk monthly calendar card for the 4.2" ZKC42V tag (400x300 BWR).

Photo-referenced layout (InkTag-style full-month grid):
  header: big date + weekday, lunar day, ganzhi year/day, week number,
          solar term, live weather from wttr.in
  grid:   Sunday-first month view, bold day numbers, weekends in red,
          today = filled circle + white number, lunar day under each cell
  footer: almanac yi/ji from lunar-python (real 黄历 data), FreeInk mark

Everything lunar/almanac related comes from the MIT-licensed `lunar-python`
library (pip). Weather is optional (network) and degrades gracefully.

Usage:
  python calendar_card.py                          # today, Suzhou
  python calendar_card.py --date 2027-01-01 --city Shanghai
  python calendar_card.py --no-weather -o out.png
"""

from __future__ import annotations

import argparse
import calendar as _cal
import datetime as dt
import json
import os
import sys
import urllib.request
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent / "_libs"))

from PIL import Image, ImageDraw, ImageFont

try:
    from lunar_python import Solar
except ImportError:
    sys.exit("need 'pip install lunar-python'")

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (200, 0, 0)

W, H, SCALE = 400, 300, 2
CW, CH = W * SCALE, H * SCALE

WEEKDAYS = "一二三四五六日"
HEADERS = "日一二三四五六"           # Sunday-first
ZODIAC = "鼠牛虎兔龙蛇马羊猴鸡狗猪"
GAN = "甲乙丙丁戊己庚辛壬癸"
ZHI = "子丑寅卯辰巳午未申酉戌亥"

CITY_CN = {"Beijing": "北京", "Shanghai": "上海", "Suzhou": "苏州",
           "Shenzhen": "深圳", "Guangzhou": "广州", "Hangzhou": "杭州",
           "Nanjing": "南京", "Chengdu": "成都", "Wuhan": "武汉",
           "Xi'an": "西安", "Chongqing": "重庆"}

WX_CODE_CN = {
    "113": "晴", "116": "多云", "119": "阴", "122": "阴", "143": "雾",
    "176": "有雨", "200": "雷阵雨", "248": "雾", "260": "冻雨", "266": "小雨",
    "296": "雨", "299": "中雨", "302": "大雨", "305": "小雨", "308": "暴雨",
    "311": "冻雨", "314": "大雨", "317": "雨夹雪", "320": "雪", "323": "阵雪",
    "326": "小雪", "329": "中雪", "332": "大雪", "335": "大雪", "338": "雪",
    "350": "冻雪", "353": "阵雨", "356": "阵雨", "359": "强阵雨", "386": "雷阵雨",
    "389": "强雷阵雨", "392": "强雷阵雨", "395": "暴雪",
}


def _pick(paths, size):
    for p in paths:
        if os.path.exists(p):
            try:
                return ImageFont.truetype(p, size * SCALE)
            except Exception:
                pass
    return None


def font(size, bold=True):
    names = ["Bold", "Regular"] if bold else ["Regular", "Bold"]
    for n in names:
        f = _pick([
            os.path.expanduser(f"~/.fonts/NotoSansCJKsc-{n}.otf"),
            f"C:/Windows/Fonts/msyh{'bd' if n == 'Bold' else ''}.ttc",
            f"/usr/share/fonts/opentype/noto/NotoSansCJK-{n}.ttc",
        ], size)
        if f:
            return f
    return ImageFont.load_default()


def lunar_text(lunar):
    """lunar day label; 初一 renders as the month name."""
    d = lunar.getDayInChinese()
    return (lunar.getMonthInChinese() + "月") if d == "初一" else d


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
            "city": CITY_CN.get(city, city), "desc": desc,
            "lo": today["mintempC"], "hi": today["maxtempC"],
            "now": cur["temp_C"], "hum": cur.get("humidity", "--"),
        }
    except Exception:
        return None


def draw():
    ap = argparse.ArgumentParser()
    ap.add_argument("--date", default=None)
    ap.add_argument("--city", default="Suzhou")
    ap.add_argument("--no-weather", action="store_true")
    ap.add_argument("-o", "--out", default=None)
    a = ap.parse_args()

    date = dt.date.fromisoformat(a.date) if a.date else dt.date.today()
    img = Image.new("RGB", (CW, CH), WHITE)
    d = ImageDraw.Draw(img)

    def T(x, y, s, size, fill=BLACK, anchor="la", bold=True):
        d.text((x * SCALE, y * SCALE), s, font=font(size, bold), fill=fill, anchor=anchor)

    def R(x, y, x2, y2, fill, w=1):
        d.rectangle([x * SCALE, y * SCALE, x2 * SCALE, y2 * SCALE], fill=fill)

    today_lunar = Solar.fromYmd(date.year, date.month, date.day).getLunar()
    iso = date.isocalendar()
    weekend = date.weekday() >= 5

    # ---------- header ----------
    # line A: "2026年11月" + [星期五] in red brackets if weekend
    ym_s = f"{date.year}年{date.month}月"
    T(8, 6, ym_s, 17)
    w_ym = d.textlength(ym_s, font=font(17, True)) / SCALE
    wd_s = f"[星期{WEEKDAYS[date.weekday()]}]"
    T(8 + w_ym + 4, 6, wd_s, 17, RED if weekend else BLACK)
    T(W - 8, 8, f"第{iso[1]}周", 12, BLACK, anchor="ra")

    # line B: lunar + ganzhi day + year shengxiao
    ly_gz = today_lunar.getYearInGanZhi()
    dy_gz = today_lunar.getDayInGanZhi()
    T(8, 30, f"农历{today_lunar.getMonthInChinese()}月{today_lunar.getDayInChinese()}"
             f" · {ly_gz}年·属{ZODIAC[ZHI.index(ly_gz[1])]} · {dy_gz}日", 12)

    # line C: solar term + weather
    parts = []
    nq = today_lunar.getNextJieQi(True)
    if nq:
        nd = dt.date.fromisoformat(nq.getSolar().toYmd())
        delta = (nd - date).days
        parts.append("明日" + nq.getName() if delta == 1 else
                     "今日" + nq.getName() if delta == 0 else
                     f"{delta}天后{nq.getName()}")
    wx = None if a.no_weather else get_weather(a.city)
    wx_s = f"{wx['city']} {wx['desc']} {wx['lo']}~{wx['hi']}℃" if wx else ""
    term_s = " · ".join(parts)
    T(8, 50, term_s, 12, RED)
    if wx_s:
        T(W - 8, 50, wx_s, 12, BLACK, anchor="ra")

    R(6, 66, W - 6, 67, BLACK)

    # ---------- weekday header ----------
    colw = (W - 16) / 7.0
    for i, ch in enumerate(HEADERS):
        cx = 8 + colw * i + colw / 2
        T(cx, 70, ch, 12, RED if i in (0, 6) else BLACK, anchor="ma")
    R(6, 86, W - 6, 87, BLACK)

    # ---------- month grid (Sunday-first) ----------
    first = date.replace(day=1)
    start = first - dt.timedelta(days=(first.weekday() + 1) % 7)  # back to Sunday
    gy, rowh = 89, 30.0
    for r in range(6):
        for c in range(7):
            cell = start + dt.timedelta(days=r * 7 + c)
            cx = 8 + colw * c + colw / 2
            cy = gy + rowh * r
            in_month = cell.month == date.month
            is_today = cell == date
            cl = Solar.fromYmd(cell.year, cell.month, cell.day).getLunar()

            if is_today:
                rr = 12
                d.ellipse([(cx - rr) * SCALE, (cy + 10 - rr) * SCALE,
                           (cx + rr) * SCALE, (cy + 10 + rr) * SCALE], fill=BLACK)
                T(cx, cy + 10, str(cell.day), 14, WHITE, anchor="mm")
                T(cx, cy + 24, lunar_text(cl), 9, RED, anchor="ma")
            elif in_month:
                T(cx, cy + 2, str(cell.day), 13,
                  RED if cell.weekday() >= 5 else BLACK, anchor="ma")
                T(cx, cy + 19, lunar_text(cl), 9,
                  RED if cell.weekday() >= 5 else BLACK, anchor="ma", bold=False)
            else:
                T(cx, cy + 3, str(cell.day), 11, BLACK, anchor="ma", bold=False)

    # ---------- footer: real almanac yi/ji ----------
    fy = gy + rowh * 6 + 4          # ~273
    R(6, fy - 4, W - 6, fy - 3, BLACK)

    def chip(x, y, ch):
        R(x, y, x + 13, y + 13, RED)
        T(x + 6.5, y + 6.5, ch, 10, WHITE, anchor="mm")

    yi = " ".join(today_lunar.getDayYi()[:3])
    ji = " ".join(today_lunar.getDayJi()[:2])
    chip(8, fy + 2, "宜")
    T(24, fy + 2, yi, 11, anchor="la")
    chip(200, fy + 2, "忌")
    T(216, fy + 2, ji, 11, anchor="la")
    T(W - 8, fy + 3, "FreeInk", 11, RED, anchor="ra")

    out = a.out or f"examples/calendar_{date.isoformat()}.png"
    img = img.resize((W, H), Image.Resampling.LANCZOS)
    Path(out).parent.mkdir(exist_ok=True)
    img.save(out)
    print(out)


if __name__ == "__main__":
    draw()
