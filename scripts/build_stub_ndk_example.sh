#!/data/data/com.termux/files/usr/bin/bash
# Example: build arm64 stub with Android NDK or Termux clang.
# Output stays local — do not commit libNOVA3_neon.so if mixed with proprietary APKs.

set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${OUT:-$ROOT/stub/libNOVA3_neon.so}"

if [[ -n "${NDK:-}" && -d "$NDK" ]]; then
  CXX="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang++"
  "$CXX" -shared -fPIC -O2 -o "$OUT" "$ROOT/stub/jni_stubs.cpp" -llog
else
  # Termux (device is already aarch64)
  clang++ -shared -fPIC -O2 -o "$OUT" "$ROOT/stub/jni_stubs.cpp" -llog
fi

echo "Built: $OUT"
llvm-readelf -d "$OUT" 2>/dev/null | grep NEEDED || readelf -d "$OUT" 2>/dev/null | grep NEEDED || true
nm -D "$OUT" 2>/dev/null | grep -E 'JNI_OnLoad|Java_' | head -30 || true
