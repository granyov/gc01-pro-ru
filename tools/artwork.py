#!/usr/bin/env python3
"""Presentation sheet of firmware-rendered synthetic fixtures, no device claims."""
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont
root=Path(__file__).resolve().parents[1]
assets=root/'docs/assets';assets.mkdir(exist_ok=True)
font=root/'fonts/NotoSans-SemiBold.ttf'
def f(n): return ImageFont.truetype(str(font),n)
for name in ('measurement','dose','alarm','session','events'):
    Image.open(root/'build'/f'{name}.ppm').save(assets/f'{name}.png')
w,h=1440,930
im=Image.new('RGB',(w,h),'#0b1420');d=ImageDraw.Draw(im)
d.rounded_rectangle((54,44,226,76),16,fill='#174b50');d.text((69,48),'EXPERIMENTAL 0.1',font=f(16),fill='#8cf1df')
d.text((54,93),'GC-01 Pro RU',font=f(62),fill='#e6edf3')
d.text((56,182),'Русский интерфейс. Открытые исходники. Честная статистика.',font=f(25),fill='#9aadc0')
for i,(name,label) in enumerate([('measurement','01 / Измерение'),('dose','02 / Накопленная доза'),('alarm','03 / Тревога')]):
    x=54+i*452;y=272
    d.rounded_rectangle((x-8,y-8,x+424,y+320),15,fill='#254055')
    screen=Image.open(assets/f'{name}.png').resize((416,312),Image.Resampling.NEAREST)
    im.paste(screen,(x,y));d.text((x,y+346),label,font=f(24),fill='#e6edf3')
d.line((54,706,1386,706),fill='#254055',width=2)
for i,(a,b) in enumerate([('320 × 240','Noto Sans • bitmap'),('64 КБ / 20 КБ','Flash / RAM'),('Rad Pro / Pro RU','Атрибуция сохранена')]):
    x=54+452*i;d.text((x,741),a,font=f(26),fill='#5ce0c5');d.text((x,786),b,font=f(21),fill='#9aadc0')
d.text((54,871),'Рендер реального UI-кода с демонстрационными данными. Работа на приборе ещё не проверена.',font=f(19),fill='#9aadc0')
im.save(assets/'overview.png')
