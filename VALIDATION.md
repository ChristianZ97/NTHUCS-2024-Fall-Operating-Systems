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
