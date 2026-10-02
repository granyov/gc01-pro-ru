#!/usr/bin/env python3
"""Regenerate compact GC-01 Russian bitmap fonts. MIT; converter: Gissio."""
from pathlib import Path
import subprocess, sys
root=Path(__file__).resolve().parents[1]
font = str(root/'fonts/NotoSans-SemiBold.ttf')
out = root/'firmware/src/ui/fonts'
for name, size, chars, filename in (
    ('font_small', 17, '0x20-0x22,0x25,0x28-0x3f,0x41-0x5a,0x61-0x7a,0xb1,0x401,0x410-0x44f,0x451,0x2012', 'font_small_ru_color_17_1bpp.h'),
    ('font_medium', 24, '0x28-0x29,0x2e-0x39,0x413,0x417,0x41c,0x431-0x432,0x438,0x43a,0x43c-0x43d,0x43f,0x441,0x447,0x44d,0x2012', 'font_medium_ru_color_24_1bpp.h'),
    ('font_large', 80, '0x2e,0x30-0x39,0x2012', 'font_large_color_80_1bpp.h'),
):
    subprocess.run([sys.executable, str(root/'tools/fontconv.py'), '-s', chars,
                    '-p', str(size), '-b', '1', '-n', name, font, str(out/filename)], check=True)
