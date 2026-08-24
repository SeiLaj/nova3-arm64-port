# Patch notes (descriptive only)

Apply only to a **legally owned** decoded APK on your machine. Do not publish the result
if it redistributes Gameloft assets or the original engine.

## 1. targetSdk

- File: `apktool.yml` / manifest
- Change: `targetSdkVersion` from 14 → 24 (or higher as required by the OS)

## 2. OpenSLES path

- File: `…/GL2JNIActivity.smali`
- Change: `/system/lib/libOpenSLES.so` → `/system/lib64/libOpenSLES.so`
  or replace `System.load(path)` with `System.loadLibrary("OpenSLES")`

## 3. MOGA / BDA

- File: `com/bda/controller/Controller.smali`
- Method: `b()Z` — skip `startService` / `bindService` on implicit intent; return `false`

## 4. Google Analytics

- `GoogleAnalyticsTracker.Init` → empty `return-void`
- `GameInstaller.onStart` — skip `GoogleAnalytics.getInstance` / tracker calls;
  keep installer worker thread if needed

## 5. Device identifiers

- `GLUtils/Device.getDeviceId` / `d1` → constant dummy string
- `InitDeviceValues` → no `TelephonyManager.getLine1Number` / restricted APIs
- `installer/utils/Tracking.init` → no-op

## 6. Native library name

- Ensure `LoadSoLibrary` / `LibraryToLoad` loads **`NOVA3_neon`** on arm64
  (not `NOVA3_8_neon` only present under armeabi-v7a)

## 7. License key JNI

- `GameInstaller.getPublicKey` must return a syntactically valid RSA public key
  Base64 string if LVL `LicenseChecker` still runs (use a key you control for testing;
  do not copy secrets from third parties into public repos)

## 8. arm64 stub placement

- Ship only **your** `lib/arm64-v8a/libNOVA3_neon.so` built from `stub/`
- Optionally keep original armeabi-v7a libs **offline** for research; do not push them

## 9. OBB / data

- Prefer real user data under `Android/obb/...` and `Android/data/.../files/`
- Bypass of extraction loops is a local convenience when OBB files are already present;
  document behavior in your private notes
