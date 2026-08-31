# N.O.V.A. 3 — pipeline de dados (C4)

Notas de engenharia reversa **estática** da lib ARM32 (`libNOVA3_neon.so`).
Este repositório **não** inclui a `.so`, OBB, `.gla`, chaves em claro nem um decryptor.

Endereços = **offset de ficheiro**. Ghidra: `addr_ghidra = offset + 0x10000`.

## Camadas no disco

```text
OBB (ZIP)
  menus.gla          SEND (22 B)
  menus4.gla         SB (magic 53 42)
  menus_stream.gla   stream (não SB no offset 0)
  *.glz              ZIP ofuscado (CScrambledZipReader)
```bash
cat > docs/engine-map.md << 'EOF'
# Mapa do motor ARM32 (C4)

Offsets de ficheiro. Ghidra: +0x10000. Não versionar a .so.

## glf
| Símbolo | Offset |
|---------|--------|
| JNI_OnLoad | 0x000d43e8 |
| glf::App::Run | 0x008cd19c |
| glf::App::MyInit | 0x008d0a68 |
| glf::AndroidShowLoadingScreen | 0x008d28f0 |
| glf::App::Update | 0x008d6388 |
| glf::App::Init | 0x008d643c |
| glf::Fs::LoadObfuscationMap | 0x008e41d8 |

MyInit = runtime (mem/threads), não abre .gla.

## GS_*
GS_Logo -> GS_Loading (this+0x44c = passo 0..10) -> GS_LoadMenu::LoadData

| Método | Offset |
|--------|--------|
| GS_Logo::Create | 0x35f468 |
| GS_Loading::FinishLoadingStep1 | 0x3b9770 |
| GS_Loading::Update | 0x3bf35c |
| GS_Loading::Create | 0x3c0174 |
| GS_LoadMenu::LoadData | 0x3c1950 |
