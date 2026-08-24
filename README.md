# N.O.V.A. 3 — experimental AArch64 / modern Android port notes

**This repository does not contain the game.**

It documents an experimental effort to run *N.O.V.A. 3 Near Orbit Vanguard Alliance*
(`com.gameloft.android.ANMP.GloftN3HM`) on **64-bit-only** Android devices, and holds
**original** helper code (JNI stubs, scripts, patch notes).

You must own a **legal copy** of the game. Do not ask for or commit APKs, OBBs, or the
original 32-bit engine libraries.

## Status (honest)

| Area | State |
|------|--------|
| Install on Android 14+ (`targetSdk` ≥ 24) | Done in local builds |
| arm64 stub `libNOVA3_neon.so` (JNI only) | Done |
| Installer flow (OBB check, license UI) | Mostly working with patches |
| OpenGL surface + clear (green/blue proof) | Done |
| Game data under `Android/data/.../files/` | User-provided; `levels.gla` etc. |
| **Real game engine on arm64** | **Not done** — original engine is ARM32 only |
| 32-bit translation on pure arm64 devices | Not available on tested hardware |

The on-screen “green/blue” frame proves **Java → EGL → GLES** is alive. It is **not**
the real renderer or gameplay loop from the 18 MB ARM32 library.

## What is in this repo

- `docs/` — continuity notes, JNI symbol lists, roadmap
- `stub/` — **original** minimal JNI stub sources (no Gameloft code)
- `scripts/` — Termux-oriented helpers (inspect, build hints)
- Patch **descriptions** only (no full decompiled tree)

## What must stay on your device / never be pushed

- Any `.apk` of the game or modified builds that still embed original libs/assets
- `main.*.obb` / `patch.*.obb`
- `lib/armeabi-v7a/libNOVA3_neon.so` (~18 MB engine)
- Extracted `.gla` / mesh / audio game data

See `.gitignore`.

## High-level architecture

```text
GL2JNIActivity / GameInstaller (Java + smali patches)
        │
        ▼
System.loadLibrary("NOVA3_neon")
        │
        ├── arm64 stub (this project)     → JNI_OnLoad, init/step/resize no-ops or GLES proof
        └── original armeabi-v7a engine   → full game (cannot load on arm64-only phones)
```

Original ARM32 library dynamic deps (from `readelf`):

- `libGLESv2.so`, `libOpenSLES.so`, `liblog.so`, `libc.so`, `libm.so`, `libdl.so`, `libstdc++.so`

Rough JNI surface: on the order of **~55** `Java_*` exports plus a large internal symbol table
(~4k dynamic symbols). A complete port is a large reverse-engineering effort, not a small stub.

## Typical device layout

```text
/sdcard/Android/obb/com.gameloft.android.ANMP.GloftN3HM/
  main.1050.com.gameloft.android.ANMP.GloftN3HM.obb
  patch.1070.com.gameloft.android.ANMP.GloftN3HM.obb

/sdcard/Android/data/com.gameloft.android.ANMP.GloftN3HM/files/
  levels.gla, actors*.gla, sounds_*.gla, sprites*.gla, ...
```

OBB content is a standard ZIP of engine assets (`.bdae`, `.etc`, …).

## Building the stub (outline)

Requirements: Android NDK or Termux `clang` targeting `aarch64-linux-android`.

```bash
# Example only — adjust sysroot / API level for your NDK
"$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang++" \
  -shared -fPIC -O2 \
  -o libNOVA3_neon.so \
  stub/jni_stubs.cpp \
  -llog
```

Inject into a **user-owned** decoded APK under `lib/arm64-v8a/`, rebuild/sign with your own
keystore. Never publish the resulting APK if it still contains Gameloft assets or the ARM32 engine.

## License

- **Documentation and original stub/scripts in this repository:** MIT (see `LICENSE`).
- **N.O.V.A. 3, Gameloft code, art, audio, and data:** © Gameloft — all rights reserved.
  Not covered by this license.

## Disclaimer

This project is for technical documentation and personal experimentation with a legally
obtained copy. Authors are not affiliated with Gameloft. No warranty. Do not use this
material to pirate or redistribute the game.
