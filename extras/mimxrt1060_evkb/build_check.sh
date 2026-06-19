#!/usr/bin/env bash
# Compile+link the teensy4 core for a given board, to verify a clean build.
# Usage: build_check.sh <MCU_DEF> <MCU_LD>
#   e.g. build_check.sh ARDUINO_MIMXRT1060_EVKB imxrt1060_evkb.ld
set -euo pipefail
ARDUINOPATH="${ARDUINOPATH:-/Applications/Teensyduino.app/Contents/Java}"
DEF="$1"; LD="$2"
HERE="$(cd "$(dirname "$0")" && pwd)"
CORE="$HERE/../../teensy4"
cd "$CORE"
# the cross-gcc finds -larm_cortexM7lfsp_math via LIBRARY_PATH
export LIBRARY_PATH="$ARDUINOPATH/hardware/tools/arm/arm-none-eabi/lib"
# minimal sketch supplies setup()/loop() so main.cpp's main() links
cat > _buildcheck_sketch.cpp <<'EOF'
#include <Arduino.h>
void setup() { pinMode(LED_BUILTIN, OUTPUT); }
void loop() { digitalWriteFast(LED_BUILTIN, HIGH); delay(100); digitalWriteFast(LED_BUILTIN, LOW); delay(100); }
EOF
trap 'rm -f _buildcheck_sketch.cpp *.o *.d main.elf' EXIT
make -s ARDUINOPATH="$ARDUINOPATH" MCU=IMXRT1062 MCU_DEF="$DEF" MCU_LD="$LD" main.elf
echo "BUILD OK: $DEF / $LD"
