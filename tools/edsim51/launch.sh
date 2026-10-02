#!/usr/bin/env bash
set -euo pipefail
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
java_bin=""
if [[ -n "${JAVA_HOME:-}" && -x "$JAVA_HOME/bin/java" ]]; then
  java_bin="$JAVA_HOME/bin/java"
else
  for candidate in /opt/homebrew/opt/openjdk/bin/java /usr/local/opt/openjdk/bin/java; do
    if [[ -x "$candidate" ]]; then java_bin="$candidate"; break; fi
  done
fi
if [[ -z "$java_bin" ]]; then java_bin="$(command -v java || true)"; fi
if [[ -z "$java_bin" ]] || ! "$java_bin" -version >/dev/null 2>&1; then
  echo "Java runtime not found. Install Java or set JAVA_HOME; see tools/edsim51/README.md." >&2
  exit 1
fi
for required in edsim51di.jar lib/edsim51sh.jar; do
  [[ -f "$script_dir/runtime/$required" ]] || { echo "Missing runtime/$required" >&2; exit 1; }
done
if [[ "${1:-}" == "--check" ]]; then
  "$java_bin" -version
  echo "EdSim51 2.1.38 runtime is ready."
  exit 0
fi
cd "$script_dir/runtime"
exec "$java_bin" -jar edsim51di.jar
