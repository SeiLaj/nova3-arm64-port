# Superfície JNI completa (sec12-ac)

Stub arm64 que **exporta** os `Java_*` listados em `jni_from_nm.txt` / `jni-symbols-arm32.txt`.

## Objectivo

Evitar `UnsatisfiedLinkError` quando o Java chama símbolos que o stub mínimo não tinha.
**Não** implementa o motor (~18 MB ARM32).

## Contagem

| Grupo | Símbolos no stub |
|-------|------------------|
| GL2JNILib | ~32 |
| GameInstaller | 4 |
| GDRMPolicy | 7 |
| InAppBilling + `iab_s_*` | 9 |
| Push C2DM | 3 |
| **Total Java_*** | **~55** |

## Build + inject

```bash
cd /storage/emulated/0/Download/nova3-arm64-port
# copiar stub/jni_stubs.cpp do pacote A+C

clang++ -shared -fPIC -O2 -std=c++17 \
  -o out/arm64-v8a/libNOVA3_neon.so stub/jni_stubs.cpp -llog && echo COMPILE_OK

strings out/arm64-v8a/libNOVA3_neon.so | grep -E 'sec12-ac|GDRMPolicy|GamePause' | head

mkdir -p $HOME/n3lib/lib/arm64-v8a
cp -f out/arm64-v8a/libNOVA3_neon.so $HOME/n3lib/lib/arm64-v8a/

cd /storage/emulated/0/Download
cp -f nova3-sec12z.apk nova3-sec12ac-unsigned.apk 2>/dev/null || \
  cp -f nova3-sec12z2-unsigned.apk nova3-sec12ac-unsigned.apk

zip -d nova3-sec12ac-unsigned.apk lib/arm64-v8a/libNOVA3_neon.so 2>/dev/null || true
cd $HOME/n3lib
zip -0 -u /storage/emulated/0/Download/nova3-sec12ac-unsigned.apk lib/arm64-v8a/libNOVA3_neon.so

unzip -l /storage/emulated/0/Download/nova3-sec12ac-unsigned.apk | grep libNOVA3_neon
# 1 linha, Stored

cd /storage/emulated/0/Download
apksigner sign --ks nova3-debug.keystore --ks-pass pass:android --key-pass pass:android \
  --out nova3-sec12ac.apk nova3-sec12ac-unsigned.apk

su -c "cp -f /storage/emulated/0/Download/nova3-sec12ac.apk /data/local/tmp/n.apk"
su -c "chmod 644 /data/local/tmp/n.apk"
su -c "pm install -r /data/local/tmp/n.apk"

LIB=$(su -c "find /data/app -path '*GloftN3HM*libNOVA3_neon.so' | head -1")
su -c "strings \"$LIB\"" | grep -F 'sec12-ac'
```

## Teste

```bash
su -c "am force-stop com.gameloft.android.ANMP.GloftN3HM"
su -c "logcat -c"
am start -n com.gameloft.android.ANMP.GloftN3HM/.GL2JNIActivity
sleep 12
echo "PID=$(su -c 'pidof com.gameloft.android.ANMP.GloftN3HM' || echo morto)"
su -c "logcat -d" | grep -iE 'NOVA3_stub|UnsatisfiedLinkError|FATAL' | tail -40
```

Esperado: `JNI_OnLoad — sec12-ac…`, **sem** `UnsatisfiedLinkError`.

## Assinaturas

Os tipos de argumentos são **best-effort**. Se o log mostrar:

```text
No implementation found for …
```

abre o smali do método (`apktool`) e ajusta `jint`/`jfloat`/`jstring`/`jobject` no stub.

## Limite honesto

```text
Símbolos presentes     ✅
Motor / gameplay       ❌
Ecrã continua verde    (esperado)
```
