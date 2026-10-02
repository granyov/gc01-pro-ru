#!/usr/bin/env python3
"""Fail if firmware binaries, secrets, or owner backup directories enter Git."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[1]
files=subprocess.check_output(['git','ls-files','-z'],cwd=root).decode().split('\0')
for f in filter(None,files):
    p=Path(f)
    assert p.suffix.lower() not in {'.bin','.elf','.map'}, f
    assert not any(x in p.parts for x in ('.pio','private','.venv')), f
    if p.suffix.lower() in {'.c','.h','.py','.md','.yml','.ini','.json','.txt'}:
        text=(root/p).read_text(errors='replace')
        assert ('-----BEGIN ' + 'PRIVATE KEY-----') not in text, f
print('publication audit: no tracked binaries, private backups or build directories')
