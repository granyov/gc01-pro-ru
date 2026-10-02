#!/usr/bin/env python3
"""Regenerate GC-01 Russian small font. MIT; converter: Gissio."""
from pathlib import Path
import subprocess, sys
root=Path(__file__).resolve().parents[1]
subprocess.run([sys.executable,str(root/'tools/fontconv.py'),
 '-s','0x20-0x22,0x25,0x28-0x3f,0x41-0x5a,0x61-0x7a,0xb1,0x401,0x410-0x44f,0x451,0x2012',
 '-p','21','-b','1','-n','font_small',str(root/'fonts/NotoSans-SemiBold.ttf'),
 str(root/'firmware/src/ui/fonts/font_small_ru_color_21_1bpp.h')],check=True)
