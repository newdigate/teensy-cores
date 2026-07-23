# tools/attic — retired tooling

## `gen_imxrt1176_h.py` (retired 2026-07-23)

`../../imxrt1176.h` was originally **bootstrapped** by this script: it parsed
peripheral base addresses out of the NXP CMSIS header
(`MIMXRT1176_cm7_COMMON.h`) and emitted a standalone, SDK-free register header
in the Teensy idiom.

It is **retired**. Over time the header was hand-edited for many peripheral
additions that were never mirrored back into the generator, so the two drifted
apart — and the generator was never wired into any build, gate, or CI, so the
drift went unenforced. Regenerating over the committed header would silently
**drop** the hand-added content and break the build. At the time of retirement
that content included (non-exhaustive):

- `ADC_CMDL_ABSEL` — used by `analog.c` for B-side ADC input select;
- the position of the `DAC12` block (moved to the end of the header — the
  generator emitted it mid-file, right after LPADC);
- the entire `PXP` block (base `0x40814000`, IRQ 57, LPCG127).

Beyond a handful of base-address and GPIO/CCM loops, the script was almost
entirely hardcoded string literals, so it offered essentially no generative
value for this fixed silicon part — editing register defs in Python literals is
strictly more error-prone than editing the `.h` directly.

### The header is now the single source of truth

Add new peripherals **directly** to `../../imxrt1176.h`, following its existing
per-peripheral comment-banner style.

### Why this file is kept

Historical provenance, and as a starting template if a future NXP part ever
needs the same bootstrap. Its `__main__` entry point is **disabled** so it can
never silently overwrite the header — running it prints an explanation and exits
non-zero. (`main()` is left intact for anyone who deliberately wants to reuse
the generation logic for a new part.)
