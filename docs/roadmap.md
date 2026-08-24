# Roadmap

## Done (local experimental builds)

1. Modern Android install (`targetSdk` 24+)
2. OpenSLES path 64-bit / `loadLibrary` fixes
3. MOGA implicit `startService` bypass
4. Google Analytics / EasyTracker crash bypasses
5. Telephony / `getDeviceId` / `getLine1Number` / `InitDeviceValues` stubs
6. arm64 `libNOVA3_neon.so` stub with core JNI entry points
7. GameInstaller: force correct library name (`NOVA3_neon`), OBB present bypass
8. License public key non-empty for LVL parser
9. GL surface + clear (proof of frame loop)
10. Confirmed game data present under `files/` when user supplies it
11. Confirmed pure arm64 device: no 32-bit translation

## Not done

| ID | Item | Notes |
|----|------|--------|
| C3 | Real engine on arm64 | Requires port/reimplementation of ~18 MB ARM32 `.so` |
| — | Full JNI parity with ARM32 exports | ~55 `Java_*` + huge internal code |
| — | Audio / gameplay / networking of original | Depends on C3 |
| — | Automated APK publish | Intentionally out of scope (copyright) |

## Suggested next research steps

1. Complete JNI catalog from ARM32 `nm -D` vs smali `native` methods.
2. Map `setPaths` / resource loading strings in the ARM32 binary.
3. Optional: implement more stub JNI only for stability (still not the game).
4. Keep proprietary binaries offline; only commit text exports of symbols.
