#include "nova3_io.h"
#include <cstring>

// TEA: delta = 0x9E3779B9, 32 rounds
// keys já ofuscadas: k[i] = ROR(raw[i] ^ 0xF3042911, 11)
static const uint32_t TEA_DELTA = 0x9E3779B9;

void tea32_decrypt(uint32_t *v, const uint32_t k[4]) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t sum = 0xC6EF3720; // delta * 32
    for (int i = 0; i < 32; i++) {
        v1 -= ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
        v0 -= ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        sum -= TEA_DELTA;
    }
    v[0] = v0; v[1] = v1;
}

void unscramble(uint8_t *buf, size_t len, const char *key) {
    if (!buf || !key || len == 0) return;
    size_t klen = strlen(key);
    if (klen == 0) return;
    for (size_t i = 0; i < len; i++) buf[i] ^= key[i % klen];
}
