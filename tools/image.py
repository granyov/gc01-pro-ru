#!/usr/bin/env python3
"""Validate ELF and produce a local-only application image; never touches hardware."""
import argparse, hashlib, json, struct
from pathlib import Path
BASE=0x08004000
SIZE=0xB000
STATE=BASE+SIZE
END=0x08010000

def crc_stm32(data):
    if len(data)%4: raise ValueError('CRC input must contain complete words')
    crc=0xffffffff
    for (word,) in struct.iter_unpack('<I',data):
        crc ^= word
        for _ in range(32):
            crc=((crc<<1)^ (0x04c11db7 if crc&0x80000000 else 0))&0xffffffff
    return crc

def validate_raw(raw):
    if not 64 <= len(raw) <= SIZE-4: raise ValueError('Invalid application length')
    sp,reset=struct.unpack_from('<II',raw)
    if sp%8 or not 0x20000000<sp<=0x20005000: raise ValueError('Invalid initial stack')
    if not reset&1 or not BASE<=(reset&~1)<BASE+len(raw): raise ValueError('Invalid reset vector')
    # Cortex-M vectors 7..10 and 13 are reserved; 8/9 hold Rad Pro metadata.
    for i in range(2,min(66,len(raw)//4)):
        if i in (7,8,9,10,13): continue
        value=struct.unpack_from('<I',raw,i*4)[0]
        if value and (not value&1 or not BASE<=(value&~1)<BASE+len(raw)):
            raise ValueError(f'Invalid vector {i}')
    # Word 66 is the CMSIS BootRAM marker, not an interrupt handler.
    if len(raw) >= 268 and struct.unpack_from("<I",raw,264)[0] != 0xf108f85f:
        raise ValueError("Invalid CMSIS BootRAM marker")
    return sp,reset

def pack(raw):
    validate_raw(raw)
    image=bytearray(b'\xff'*SIZE)
    image[:len(raw)]=raw
    struct.pack_into('<II',image,0x20,STATE,0x400)
    struct.pack_into('<I',image,SIZE-4,crc_stm32(image[:-4]))
    return image

def inspect_elf(path,raw):
    from elftools.elf.elffile import ELFFile
    with open(path,'rb') as f:
        elf=ELFFile(f)
        if elf['e_machine']!='EM_ARM' or elf.elfclass!=32 or not elf.little_endian:
            raise ValueError('Expected little-endian ARM ELF32')
        vectors=elf.get_section_by_name('.isr_vector')
        if vectors is None or vectors['sh_addr']!=BASE: raise ValueError('Wrong vector origin')
        if vectors.data()!=raw[:vectors['sh_size']]: raise ValueError('ELF/bin vectors differ')
        memory=0
        for seg in elf.iter_segments():
            if seg['p_type']!='PT_LOAD':continue
            a,n=seg['p_paddr'],seg['p_filesz']
            if n and not (BASE<=a and a+n<=STATE-4):raise ValueError('Load segment outside app')
            if n and seg.data()!=raw[a-BASE:a-BASE+n]:raise ValueError('ELF/bin payload differs')
            v,m=seg['p_vaddr'],seg['p_memsz']
            if 0x20000000<=v<0x20005000:
                if v+m>0x20005000:raise ValueError('RAM overflow')
                memory=max(memory,v+m-0x20000000)
        if elf['e_entry']!=struct.unpack_from('<I',raw,4)[0]:raise ValueError('Entry mismatch')
        return memory

def main():
    p=argparse.ArgumentParser();p.add_argument('--build',default='firmware/.pio/build/gc01-pro-ru');p.add_argument('--out',default='build/experimental');args=p.parse_args()
    build=Path(args.build);out=Path(args.out);out.mkdir(parents=True,exist_ok=True)
    raw=(build/'firmware.bin').read_bytes();sp,reset=validate_raw(raw)
    ram=inspect_elf(build/'firmware.elf',raw);img=pack(raw)
    name='gc01-pro-ru-v0.1.0-EXPERIMENTAL-app.bin';(out/name).write_bytes(img)
    report=dict(target='GC01 V0.2 / CH32F103R8T6',status='BUILD_ONLY_NOT_HARDWARE_VALIDATED',
        origin=hex(BASE),application_bytes=len(raw),application_budget=SIZE-4,
        image_bytes=len(img),ram_reserved_end_bytes=ram,ram_total=20480,
        initial_sp=hex(sp),reset_vector=hex(reset),settings_base=hex(STATE),
        history_bytes=END-STATE-0x400,sha256=hashlib.sha256(img).hexdigest(),
        public_binary_release_allowed=False,
        unresolved=['Owner stock firmware and PCB image unavailable in this task',
                    'Bootloader identity / transfer semantics unverified on device',
                    'HV, pulse calibration, RAM stack high-water and peripherals untested'])
    (out/'build-report.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n')
    print(json.dumps(report,ensure_ascii=False,indent=2))
if __name__=='__main__':main()
