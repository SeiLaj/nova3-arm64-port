#pragma once
#include <cstdint>
#include <cstddef>

// TEA-32 decrypt (bloco de 8 bytes)
void tea32_decrypt(uint32_t *block, const uint32_t keys[4]);

// XOR unscramble (.glz)
void unscramble(uint8_t *buf, size_t len, const char *key);

// SB walker (o que já tinhas no stub)
struct SBRecord { uint16_t tag; uint32_t size; uint8_t name[6]; };
bool sb_walk(const uint8_t *data, size_t len, SBRecord *out);
