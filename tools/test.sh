#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build
cc -std=c11 -Wall -Wextra -Werror tests/core_test.c firmware/src/system/session.c firmware/src/system/cmath.c -lm -o build/core-test
./build/core-test
python3 -m unittest discover -s tests -p 'test_*.py' -v
