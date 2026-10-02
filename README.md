# Operating Systems Portfolio

Four project checkpoints for NTHU CS342302 Operating Systems, Fall 2024. Implements cooperative and preemptive threading and semaphore synchronization on an 8051 target.

## Quick Start

```sh
make -C assignments/checkpoint01
./tools/edsim51/launch.sh
```

Java and SDCC are required. Load `assignments/checkpoint01/testcoop.hex` in the simulator. See [EdSim51 setup and examples](tools/edsim51/) for macOS double-click launch and Windows/Linux instructions.

## Projects

| Project | Technology |
| --- | --- |
| [checkpoint01: Cooperative Multithreading](assignments/checkpoint01/) | C / SDCC / 8051 |
| [checkpoint02: Timer-Based Preemption](assignments/checkpoint02/) | C / SDCC / 8051 |
| [checkpoint03: Semaphore Synchronization](assignments/checkpoint03/) | C / SDCC / 8051 |
| [checkpoint04: Three-Thread Synchronization](assignments/checkpoint04/) | C / SDCC / 8051 |

## Repository Layout

```text
assignments/     Browsable implementations and per-project instructions
  */submission/ Byte-exact extracted submission files
SUBMISSIONS.md  Submission inventory and SHA-256 hashes
CURATION.md     Source provenance and publication boundary
VALIDATION.md   Local verification results
```

## Reproducibility

There is no root build. Enter an individual project directory and follow its README. Source code is preserved without functional changes. Original reports and source files may retain names and student identifiers.

## Academic Use

Coursework preserved as a portfolio and learning reference. Follow your institution’s academic-integrity rules. Existing course-template attribution is retained. No blanket license is granted for third-party materials or for this collection.
