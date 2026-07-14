# License

## imxrt1176 core — MIT (fully permissive)

Everything under `imxrt1176/` is MIT-licensed: PJRC-originated files carry
their original MIT headers; the RT1176 port and the 2026-07-13 clean-room
replacements (`Printable.h`, `Stream.h/.cpp`, `WString.h/.cpp`, `Time.cpp`,
`nonstd.c`, plus the earlier `WCharacter.h` and `WMath.cpp`) are
Copyright (c) 2026 Nicholas Newdigate, MIT (SPDX headers in each file).
The clean-room files were written from the documented Arduino API surface
and extracted interface contracts, not derived from the LGPL originals —
process and provenance are recorded in
`evkb/docs/superpowers/specs/2026-07-13-rt1176-license-cleanroom-design.md`.

No copyleft code is compiled into RT1176 firmware. This is enforced
mechanically by `evkb/tools/license-audit.sh`: a wrap-tolerant repo sweep
plus a link-manifest audit that walks the CMake depfiles of three fat gate
builds and checks the license header of every source and header that
actually feeds an object, with dual-licensed platform files verified to
compile to empty objects.

## teensy/, teensy3/, teensy4/ — upstream reference copies (NOT compiled)

These directories are unmodified PJRC Teensyduino cores kept for reference
and diffing. They retain their upstream licenses (MIT and LGPL-2.1 per file
header) and are never part of any RT1176 build — the link-manifest audit
proves nothing under them is compiled.
