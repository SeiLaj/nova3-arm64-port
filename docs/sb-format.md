# Formato SB (`.gla`) — checkpoint C3.0 → C3.11

Notas de engenharia reversa **originais** sobre os pacotes de dados
em `Android/data/.../files/*.gla` usados pelo port experimental arm64.
**Não** inclui código, assets nem binários da Gameloft.

## Contexto

| Item | Estado |
|------|--------|
| Instalador + OBB + licença | OK (patches smali / stubs) |
| `files/` populado | OK (`levels.gla`, `actors*.gla`, …) |
| Sonda nativa `FOUND` | OK (sec12z2) |
| Walker SB record-a-record | OK (C3.10) |
| Payload cookie/trailer | OK (C3.11) |
| Motor / decode do miolo | **Não** — fica na lib ARM32 ~18 MB |

O ecrã verde do stub **não** é falta de dados: é falta do binário do jogo em arm64.

## Header de record

Cada record começa com **10 bytes**:

```text
offset  size  campo
0       2     magic  = 0x53 0x42  ('S''B')
2       2     tag    = uint16 LE  (tipo de chunk; observados 34–73 em levels)
4       6     nome   = 6 bytes XOR máscara fixa
```

### Máscara do nome

```text
kMask[6] = { 0x14, 0x00, 0x00, 0x00, 0x08, 0x00 }
clear[i] = raw[4+i] XOR kMask[i]
```

Após XOR, o nome é ASCII alfanumérico curto (ex.: `levels`, `actors`).

### Tamanho do record

**Não** há campo `u32 size` fiável no header nem nos primeiros 32 B do payload.

```text
size(record_i) = offset(record_{i+1}) − offset(record_i)
size(último)   = filesize − offset(último)
```

O walker varre o ficheiro por magic `SB` + tag &lt; 200 + nome XOR válido.

## Estrutura típica de um pack

Exemplo observado em `levels.gla` (~23.6 MB, ~758 records):

| offset | tag | size aprox. | papel |
|--------|-----|-------------|--------|
| 0 | 43 | ~151 | header do pack |
| 151 | 50 | ~131 | entrada TOC curta |
| 282 | 43 | ~151 | TOC |
| 433 | 50 | ~131 | TOC |
| 564 | 42 | ~157 KB | 1.º payload grande |
| … | 34–73 | variável | blobs |

- Record 0 = contentor (tag do pack).
- Tags = tipo de chunk (dezenas de ids, não um enum de 4 valores).
- `actors.gla`: chão de size ~1 KB; header maior no record 0.

## Payload (depois dos 10 B)

Os primeiros ~32 B do payload **não** são GOL/ZIP/zlib.

Padrão observado (C3.11):

```text
[ cookie 4 B  — constante / família por pack ]
[ 4–8 B variáveis ]
[ nome ofuscado outra vez ]
[ trailer de bytes pequenos — igual em records do mesmo .gla ]
```

### Cookies (exemplos, LE)

| Pack | cookie típico |
|------|----------------|
| levels (TOC) | `C1 CA EE 38` |
| levels (payload) | família `… EE 38` |
| actors | família `… CF EE 38` |
| weapons | `86 86 E5 35` |
| effects | `3A 30 24 2C` |

### O que **não** está no header/payload curto

| Hipótese | Resultado |
|----------|-----------|
| Tabela de offsets `u32` nos primeiros 256 B | ❌ (falsos positivos no magic) |
| Nested `SB` / `GOL` / `ZIP` nesses OFF | ❌ |
| `u32` ≈ `recsz` ou `recsz−10` no payload | ❌ |

O miolo dos payloads grandes é **opaco** (comprimido/ofuscado); o decode está no motor ARM32.

## Outros contentores

| Extensão | Nota |
|----------|------|
| `.gla` | arquivo SB (este doc) |
| `.glz` | frequentemente ZIP (`PK`) — ex. `sounds_hi.glz` |
| `oconf.bar` | ficheiro pequeno de config (~72 B) |

## Conclusão

```text
Walker SB (offset + tag + size=delta)   ✅
Nome XOR 6 B                            ✅
Cookie / trailer no payload             ✅
Significado dos tags 34–73              ❌ (motor)
Descompress/decode do miolo             ❌ (motor)
Gameplay                                ❌
```

**Não vale a pena** mais sondas C3.12+ só no stub. Próximo salto real = motor (device 32-bit ou porto arm64), ou superfície JNI completa no stub (ver `stub/jni_stubs.cpp`).

## Regenerar sondas no device

Código de probe vive no stub local (não redistribuir `.gla`/APK). Logs típicos:

```text
SB10 … nREAL=… min=… max=… avg=…
SB11 … REC@… tag=… recsz=… u32=… hex=[…]
```
