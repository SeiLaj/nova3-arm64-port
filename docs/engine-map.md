# Mapa do motor ARM32 (C4)

Offsets de ficheiro. Ghidra: +0x10000. Nao versionar a .so.

## glf

| Simbolo | Offset |
|---------|--------|
| JNI_OnLoad | 0x000d43e8 |
| glf::App::Run | 0x008cd19c |
| glf::App::MyInit | 0x008d0a68 |
| glf::AndroidShowLoadingScreen | 0x008d28f0 |
| glf::App::Update | 0x008d6388 |
| glf::App::Init | 0x008d643c |
| glf::Fs::LoadObfuscationMap | 0x008e41d8 |
| glf::CrcChecker::CRC32 | 0x008dbad4 |

MyInit = runtime (mem/threads), nao abre .gla.

## GS_*

GS_Logo -> GS_Loading (this+0x44c = passo 0..10) -> GS_LoadMenu::LoadData

| Metodo | Offset |
|--------|--------|
| GS_Logo::Create | 0x35f468 |
| GS_Loading::FinishLoadingStep1 | 0x3b9770 |
| GS_Loading::Update | 0x3bf35c |
| GS_Loading::Create | 0x3c0174 |
| GS_LoadMenu::LoadData | 0x3c1950 |

## I/O + crypto

| Simbolo | Offset |
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
