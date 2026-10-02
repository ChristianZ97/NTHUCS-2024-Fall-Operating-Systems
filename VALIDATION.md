# Local Validation

Run on 2026-10-02 in temporary copies. Compilation does not establish simulator correctness. SPIM runs are smoke checks with fixed stdin, not grading tests. Verilog results below are from recovered course fixtures.

## checkpoint01: `make`

Exit status: 0.

```text
sdcc -c  testcoop.c
sdcc -c  cooperative.c
sdcc  -o testcoop.hex testcoop.rel cooperative.rel
cooperative.c:214: warning 85: in function ThreadCreate unreferenced function argument : 'fp'

```

## checkpoint02: `make`

Exit status: 0.

```text
sdcc -c  testpreempt.c
sdcc -c  preemptive.c
sdcc  -o testpreempt.hex testpreempt.rel preemptive.rel
preemptive.c:116: warning 85: in function ThreadCreate unreferenced function argument : 'fp'

```

## checkpoint03: `make`

Exit status: 0.

```text
sdcc -c  testpreempt.c
sdcc -c  preemptive.c
sdcc  -o testpreempt.hex testpreempt.rel preemptive.rel
preemptive.c:120: warning 85: in function ThreadCreate unreferenced function argument : 'fp'

```

## checkpoint04: `make`

Exit status: 0.

```text
sdcc -c  test3threads.c
sdcc -c  preemptive.c
sdcc  -o test3threads.hex test3threads.rel preemptive.rel
preemptive.c:113: warning 85: in function ThreadCreate unreferenced function argument : 'fp'

```

## EdSim51 launcher

`bash -n tools/edsim51/launch.sh` and `./tools/edsim51/launch.sh --check` passed with OpenJDK 23.0.2. Both bundled JAR ZIP integrity checks passed. GUI and Windows launch were not exercised.

## Showcase formatting verification — 2026-10-03

All 12 showcase C/header files retain the same lexical token sequence after removing comments and whitespace. Original `submission/` files remain byte-identical. Before/after builds in temporary directories succeed, with byte-identical HEX output for all four checkpoints:

| Checkpoint | HEX SHA-256 (before and after) |
| --- | --- |
| 01 | `d9457922d3b04f56179990f1c8f803b4fd5338db447db71b6808f54ad8aa79ad` |
| 02 | `411e061e6489178bfc8ebaec8fe0a1b7b6e9babe7afd49a1b64f13f21d34c39e` |
| 03 | `0496d06358c4c47baf5cd9556697c9edc9ddf713b2939af08afbeff510bbcb88` |
| 04 | `b8c95dd60dfb28a58c973ae4a9605180bb903f76d1a3b2e738f8c1671bf0e8ab` |

`.clang-format` describes the C style. SDCC assembly and multiline assembly macros are explicitly protected from the C formatter. These checks establish preservation of compiled behavior, not correctness of the original algorithms.
