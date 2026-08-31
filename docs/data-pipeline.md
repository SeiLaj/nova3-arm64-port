# N.O.V.A. 3 — pipeline de dados (C4)

Notas de engenharia reversa **estática** da lib ARM32 (`libNOVA3_neon.so`).
Este repositório **não** inclui a `.so`, OBB, `.gla`, tabela CRC, chaves TEA nem um decryptor.

Endereços = **offset de ficheiro**. Ghidra: `addr_ghidra = offset + 0x10000`.

## Camadas no disco

```text
OBB (ZIP)
  menus.gla          SEND (22 B) — ASCII "SEND" + nome repetido
  menus4.gla         SB (magic 53 42, tag 0x0028, nome XOR → menus4)
  menus_stream.gla   stream (não SB no offset 0)
  *.glz              ZIP ofuscado (CScrambledZipReader)
path
  -> CCustomFileSystem::fastCreateAndOpenFile     @ 0x2a0cf8
      -> CCustomPakReader::scanLocalHeader        @ 0x2a089c
         header 16 B = 4× int32; N = int3;
         tabela N × 16 B  (não é PK\x03\x04)
      -> CScrambledZipReader::unscramble          @ 0x4131c0
         XOR cíclico com string (.glz)
  -> Application::DecryptAndLoad                  @ 0x5a081c
      1. open file
      2. lê u32 = tamanho do payload
      3. payload <= GetSize() − 8
      4. lê payload para CMemoryStream
      5. CEncryption::CheckIntegrity              @ 0x5bd150
      6. CEncryption::EncryptBuffer(flag=0, …)    @ 0x5c758c
  -> CMemoryStream
  -> GS_LoadMenu::LoadData                        @ 0x3c1950
      ReadShort 0x4E56, depois ver 2 ou 3
```bash
cat > docs/engine-map.md << 'EOF'
# Mapa do motor ARM32 (C4)

Offsets de ficheiro. Ghidra: +0x10000. Não versionar a `.so`.

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
| glf::CrcChecker::CRC32 | 0x008dbad4 |

MyInit = runtime (mem/threads), não abre `.gla`.

## GS_*

GS_Logo → GS_Loading (this+0x44c = passo 0..10) → GS_LoadMenu::LoadData

| Método | Offset |
|--------|--------|
| GS_Logo::Create | 0x35f468 |
| GS_Loading::FinishLoadingStep1 | 0x3b9770 |
| GS_Loading::Update | 0x3bf35c |
| GS_Loading::Create | 0x3c0174 |
| GS_LoadMenu::LoadData | 0x3c1950 |

## I/O + crypto

| Símbolo | Offset |
|---------|--------|
| CCustomPakReader::scanLocalHeader | 0x2a089c |
| CCustomFileSystem::fastCreateAndOpenFile | 0x2a0cf8 |
| CScrambledZipReader::unscramble | 0x4131c0 |
| Application::DecryptAndLoad | 0x5a081c |
| CEncryption::CheckIntegrity | 0x5bd150 |
| CEncryption::EncryptBuffer | 0x5c758c |
| CEncryption::Init | 0x5c77c8 |
| CEncryption::C1 | 0x5c78e4 |
| ComputeCRC | 0x0dec20 |
| g_nCRCTable | 0xc4596c |
