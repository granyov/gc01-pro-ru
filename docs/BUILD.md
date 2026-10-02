# Сборка и воспроизводимость

Поддерживаемая цель одна: `gc01-pro-ru`, CH32F103R8T6, русский UI.
Для CLI подходят Linux/macOS; Windows — через подходящее окружение Python/PlatformIO.
CI использует Ubuntu 24.04 и Python 3.12. Локальная сборка проверена на macOS arm64.

```sh
python3 -m venv .venv
. .venv/bin/activate
python -m pip install -r requirements.txt
make test
make image
```

Первый запуск загружает закреплённые пакеты PlatformIO. Для изоляции кеша можно задать
`PLATFORMIO_CORE_DIR` в рабочем каталоге. Никакие команды выше не обращаются к прибору.
В Windows PowerShell окружение активируется через `.venv\Scripts\Activate.ps1`;
вместо make можно выполнить `pio run -d firmware`, затем `python tools/image.py`.
Тестовый shell-скрипт требует POSIX shell и native C compiler.

| Зависимость | Версия |
|---|---|
| PlatformIO Core | 6.1.18 |
| ststm32 | 19.0.0 |
| ARM GCC package | 1.120301.0 / GCC 12.3.1 |
| CMSIS core | 2.50501.200527 |
| CMSIS STM32F1 | 4.3.5 |
| ldscripts / SCons | 0.2.0 / 4.40801.0 |
| pyelftools | 0.32 |

`platformio.ini` закрепляет версии. Сборка использует `-Os`, LTO, soft-float, newlib,
собственный startup и linker script. Старый GCC 7 из стандартного выбора PlatformIO
не используется: он несовместим с нативным Apple Silicon окружением.

## Артефакты

* `firmware/.pio/build/gc01-pro-ru/firmware.elf` — код, символы и сегменты.
* `firmware/.pio/build/gc01-pro-ru/firmware.bin` — сырой образ приложения без footer CRC.
* `build/experimental/gc01-pro-ru-v0.2.0-EXPERIMENTAL-app.bin` — локальная проверочная упаковка.
* `build/experimental/build-report.json` — размер, SHA-256, адреса, причины запрета релиза бинарника.

Проверяются ELF32/ARM/little-endian, начало векторов, stack pointer, Thumb reset/IRQ-векторы,
совпадение сегментов ELF с `.bin`, отсутствие загрузочных сегментов вне приложения,
RAM-граница, метаданные настроек и CRC. Система не использует `eval` над метаданными ELF.

CI делает чистую пересборку и сравнивает application images побайтно. Воспроизводимость
здесь означает идентичность двух сборок с закреплённым toolchain; межплатформенная
идентичность проверяется отдельно. Отладочные ELF/пути могут различаться. CI сохраняет
**только отчёт и изображения UI**. `.bin`/`.elf` не прикладываются к публичным artifacts.

## Интерфейс и шрифты

```sh
python -m pip install -r requirements-ui.txt
make ui
# Необязательно: пересоздать все три шрифта из включённого TTF
python tools/build-fonts.py
pio run -d firmware -t clean
make image
```

`make ui` компилирует настоящий C-рендерер UI и задаёт синтетические значения.
Это статические сценарии, не полноценная эмуляция MCU. Тестовый framebuffer размещён
только на компьютере; целевой драйвер использует текстовый буфер.
Noto Sans: малый 17 px (вся кириллица и используемый набор ASCII), единицы 24 px, большие цифры 80 px;
bitmap 1 bpp экономит Flash. Заголовки шрифтов уже включены в исходники,
повторная растеризация разными версиями FreeType может дать другие байты.
После генерации шрифтов нужна чистая сборка, чтобы новый bitmap точно вошёл в образ.
