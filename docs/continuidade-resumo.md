# Continuity summary (sec12 line → modern Android)

Condensed from private session logs. No binaries included.

## Package

- Application ID: `com.gameloft.android.ANMP.GloftN3HM`
- Main activity: `.GL2JNIActivity`
- Installer: `.installer.GameInstaller`

## Crash → fix chain (illustrative local builds)

| Theme | Symptom | Direction of fix |
|-------|---------|------------------|
| SDK | `INSTALL_FAILED_DEPRECATED_SDK_VERSION` (14) | `targetSdkVersion` 24 |
| OpenSLES | `dlopen /system/lib/libOpenSLES.so` | `lib64` or `loadLibrary("OpenSLES")` |
| MOGA | `Service Intent must be explicit` | `Controller.b()Z` no-op |
| Analytics | EasyTracker / `getInstance` | Init / onStart no-ops |
| Telephony | `getDeviceId` / `getLine1Number` | Dummy IDs; stub `InitDeviceValues` |
| Wrong SO name | `libNOVA3_8_neon.so` not found | Force `NOVA3_neon` for arm64 |
| License | empty / invalid public key | Valid Base64 RSA string for parser |
| Linker | `libc.so.6` on stub | Build for Bionic, no glibc |
| GL proof | black screen | Java GLES clear (green/blue) |

## Confirmed environment (example device)

- ABI list: `arm64-v8a` only (no Houdini / NDK translation)
- OBB: ZIP with meshes/textures; names `main.1050.*` / `patch.1070.*`
- `files/`: populated with `levels.gla` and related `.gla` when data present

## Milestone meaning

“Installer + license + green GL clear” means the **shell** runs on modern arm64.
It does **not** mean the ARM32 game engine was ported.
