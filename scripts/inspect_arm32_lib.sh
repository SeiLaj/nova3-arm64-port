#!/data/data/com.termux/files/usr/bin/bash
# Inspect a USER-OWNED extracted ARM32 engine .so (path as argument).
# Prints text only — safe to paste into docs/jni-symbols-arm32.txt updates.

set -euo pipefail
SO="${1:?usage: $0 path/to/libNOVA3_neon.so}"

echo "===== file ====="
ls -lah "$SO"

echo "===== NEEDED ====="
readelf -d "$SO" 2>/dev/null | grep -E 'NEEDED|SONAME' || true

echo "===== JNI (head) ====="
nm -D "$SO" 2>/dev/null | grep -E ' Java_|JNI_OnLoad' | head -80

echo "===== counts ====="
echo -n "defined dynamic: "
nm -D --defined-only "$SO" 2>/dev/null | wc -l
echo -n "Java_ symbols: "
nm -D "$SO" 2>/dev/null | grep -c ' Java_' || true
