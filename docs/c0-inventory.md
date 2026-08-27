# C0 — inventário de paths / assets (arm32)

## JNI / C++ de paths
- `Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_setPaths`
- `glf::AndroidSetPaths(char const*, char const*, char const*, char const*)`

## OBB esperado
- `main.1050.com.gameloft.android.ANMP.GloftN3HM.obb`
- (patch no dispositivo: `patch.1070.com.gameloft.android.ANMP.GloftN3HM.obb`)

## Pacotes .gla (motor)
- levels.gla, actors.gla, actors_stream.gla
- sounds_hi.gla, sprites.gla, effects.gla, effects_moto.gla
- weapons.gla, weapons_stream.gla, particlesystems.gla
- options.gla, menus.gla, strings.gla, lut_android.gla
- exemplos de nível: l_002_SanFrancisco4.gla, l_100_multiplayer4.gla, l_101_coop4.gla

## JNI exports
Ver `docs/jni_from_nm.txt` (57 símbolos `Java_*` / `JNI_*`).

## Notas
- Lib arm32: \~18.6 MB, ELF, GCC/NDK antigo
- Ghidra headless no telemóvel: import OK; análise completa instável (RAM)
- Não versionar a `.so` neste repositório
