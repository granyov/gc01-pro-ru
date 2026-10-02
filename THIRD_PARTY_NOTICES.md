# Third-party notices

This is a derivative of [Rad Pro](https://github.com/Gissio/radpro), not a FNIRSI
product or an official Rad Pro release. Original headers remain in source files.
Snapshot and import scope: [UPSTREAM.md](UPSTREAM.md).

| Component | Copyright / source | License |
|---|---|---|
| Rad Pro firmware, compact math structure, tooling | © 2022–2026 Gissio | [MIT](LICENSES/RadPro-MIT.txt) |
| mcu-renderer, mcu-max, stm32 helper libraries; RadPro Symbols font | Gissio, notices in files | [MIT](LICENSES/RadPro-MIT.txt) |
| libusb_stm32, bundled source | © Dmitry Filimonchuk, Max Chan and contributors; [upstream](https://github.com/dmitrystu/libusb_stm32) | [Apache-2.0](LICENSES/Apache-2.0.txt) |
| Noto Sans SemiBold TTF and derived bitmap fonts | © 2022 The Noto Project Authors; [upstream](https://github.com/notofonts/latin-greek-cyrillic) | [SIL OFL 1.1](LICENSES/NotoSans-OFL.txt) |
| gc01.ld linker script (modified) | © 2021 STMicroelectronics | [BSD-3-Clause](LICENSES/BSD-3-Clause.txt) |
| CMSIS core/device, downloaded by PlatformIO | Arm / STMicroelectronics; pinned package versions | [Apache-2.0](LICENSES/Apache-2.0.txt), package headers |
| GCC compiler runtime, linked at build | Free Software Foundation | [GPLv3](LICENSES/GPL-3.0.txt) with [GCC Runtime Library Exception 3.1](LICENSES/GCC-Runtime-Exception.txt) |
| newlib libc/libm, linked at build | Multiple copyright holders, including Sun Microsystems for fdlibm | [Newlib notices](LICENSES/Newlib.txt) |

Changes in this project: © 2026 GC-01 Pro RU contributors, MIT. Per-file third-party
licenses continue to apply; the top-level MIT license does not relicense dependencies.

**qfplib is not included or linked.** Its GPL-2.0-only headers were identified during
import; this target uses GCC soft-float and newlib instead of combining it with the
Apache-2.0 USB stack. Residual upstream `#if QFP` branches in unrelated targets are
inactive and no qfplib source is distributed.

No proprietary FNIRSI firmware, vendor bootloader or owner backup is distributed.
Font source and conversion tooling are included; pre-generated headers are sufficient
to build firmware. Desktop tools are development dependencies, not embedded code.
