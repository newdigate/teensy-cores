#!/bin/sh
set -e
DIR=$(cd "$(dirname "$0")" && pwd); OUT=$(mktemp -d); trap 'rm -rf "$OUT"' EXIT
CXX=${CXX:-c++}
$CXX -std=c++11 -Wall -Wextra -Werror -I"$DIR/../.." "$DIR/lpuart_rts_test.cpp" -o "$OUT/t"
"$OUT/t"
echo "LPUART-RTS-HOST-TEST: PASS"
