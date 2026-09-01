#include <cstdio>
#include <cstdint>
#include <cstring>

static const uint32_t TEA_DELTA = 0x9E3779B9;

void tea32_decrypt(uint32_t *v, const uint32_t k[4]) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t sum = 0xC6EF3720;
    for (int i = 0; i < 32; i++) {
        v1 -= ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
        v0 -= ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        sum -= TEA_DELTA;
    }
    v[0] = v0; v[1] = v1;
}

void tea32_encrypt(uint32_t *v, const uint32_t k[4]) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t sum = 0;
    for (int i = 0; i < 32; i++) {
        sum += TEA_DELTA;
        v0 += ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        v1 += ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
    }
    v[0] = v0; v[1] = v1;
}

int main() {
    // Test vector: chaves dummy, dados conhecidos
    uint32_t k[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
    uint32_t v[2] = {0x12345678, 0x9abcdef0};
    uint32_t orig[2] = {v[0], v[1]};

    tea32_encrypt(v, k);
    printf("encrypted: %08x %08x\n", v[0], v[1]);
    tea32_decrypt(v, k);
    printf("decrypted: %08x %08x\n", v[0], v[1]);

    if (v[0] == orig[0] && v[1] == orig[1]) {
        printf("TEA_OK\n");
        return 0;
    } else {
        printf("TEA_FAIL\n");
        return 1;
    }
}
