#include <android/log.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>

#define LOG_TAG "NOVA3_engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern void tea32_decrypt(uint32_t *v, const uint32_t k[4]);

static const uint32_t TEA_KEYS[4] = {
    0x453465F2, 0x453477F2, 0x466465F2, 0x453433F2
};

// Ofuscar keys
static void get_tea_keys(uint32_t k[4]) {
    for (int i = 0; i < 4; i++) {
        uint32_t raw = TEA_KEYS[i];
        uint32_t x = raw ^ 0xF3042911;
        k[i] = (x >> 11) | (x << (32 - 11));
    }
}

// Aplicar TEA em todo o buffer
static void decrypt_buffer(uint8_t* buf, size_t size) {
    uint32_t k[4];
    get_tea_keys(k);
    uint32_t* data = (uint32_t*)buf;
    int blocks = size / 8;
    for (int i = 0; i < blocks; i++) {
        tea32_decrypt(&data[i*2], k);
    }
}

// Procurar NV magic em um buffer
static int find_nv_magic(const uint8_t* buf, size_t size, int max_find) {
    int found = 0;
    for (size_t i = 0; i < size - 4; i++) {
        if (buf[i] == 0x56 && buf[i+1] == 0x4E) {
            uint16_t ver = buf[i+2] | (buf[i+3] << 8);
            if (ver == 2 || ver == 3) {
                found++;
                if (found <= max_find) {
                    LOGI("  NV+ver%d at offset 0x%zx", ver, i);
                }
            }
        }
    }
    return found;
}

// Walk no SB (versão simples)
static void walk_sb(const uint8_t* buf, size_t size) {
    size_t pos = 0;
    int record_count = 0;
    int nv_found = 0;
    
    while (pos + 16 <= size) {
        // Magic SB
        if (buf[pos] != 0x53 || buf[pos+1] != 0x42) {
            LOGE("walk_sb: invalid magic at 0x%zx", pos);
            break;
        }
        
        uint16_t tag = buf[pos+2] | (buf[pos+3] << 8);
        // O tamanho pode ser calculado de várias formas; vamos usar heurística
        // Nome XOR (6 bytes)
        uint8_t name[7] = {0};
        uint8_t mask[6] = {0x14, 0x00, 0x00, 0x00, 0x08, 0x00};
        for (int i = 0; i < 6; i++) {
            name[i] = buf[pos + 8 + i] ^ mask[i];
        }
        name[6] = '\0';
        
        LOGI("walk_sb: record %d tag=0x%04x name='%s' at 0x%zx", record_count, tag, name, pos);
        
        // Pular para o próximo registro (heurística: 16 bytes header + payload)
        // O payload começa em pos+14
        size_t payload_start = pos + 14;
        // Tamanho do payload = tag? ou outro campo? Vamos tentar 4 bytes em pos+4
        uint32_t payload_size = buf[pos+4] | (buf[pos+5] << 8) | (buf[pos+6] << 16) | (buf[pos+7] << 24);
        if (payload_size > 0 && payload_size < 1024*1024) {
            // Se for um tamanho razoável, usar
            pos += 14 + payload_size;
        } else {
            // Fallback: pular 1024 bytes (heurística)
            pos += 1024;
        }
        
        record_count++;
        if (record_count > 100) break; // segurança
    }
    
    LOGI("walk_sb: total records processed: %d", record_count);
}

void test_sb_walker() {
    LOGI("test_sb_walker: starting");
    
    const char* path = "/sdcard/Android/data/com.gameloft.android.ANMP.GloftN3HM/files/menus4.gla";
    
    FILE* f = fopen(path, "rb");
    if (!f) {
        LOGE("test_sb_walker: cannot open %s", path);
        return;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    uint8_t* buf = new uint8_t[size];
    fread(buf, 1, size, f);
    fclose(f);
    
    LOGI("test_sb_walker: read %s (%ld bytes)", path, size);
    
    // Decrypt TEA
    decrypt_buffer(buf, size);
    
    // Procurar NV magic no buffer inteiro
    int nv_total = find_nv_magic(buf, size, 10);
    LOGI("test_sb_walker: total NV+ver2/3 in decrypted buffer: %d", nv_total);
    
    // Walk SB
    walk_sb(buf, size);
    
    delete[] buf;
    LOGI("test_sb_walker: done");
}
