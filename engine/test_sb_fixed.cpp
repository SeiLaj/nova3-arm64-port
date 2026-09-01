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

static void get_tea_keys(uint32_t k[4]) {
    for (int i = 0; i < 4; i++) {
        uint32_t raw = TEA_KEYS[i];
        uint32_t x = raw ^ 0xF3042911;
        k[i] = (x >> 11) | (x << (32 - 11));
    }
}

static void decrypt_buffer(uint8_t* buf, size_t size) {
    uint32_t k[4];
    get_tea_keys(k);
    uint32_t* data = (uint32_t*)buf;
    int blocks = size / 8;
    for (int i = 0; i < blocks; i++) {
        tea32_decrypt(&data[i*2], k);
    }
}

static int find_nv_magic(const uint8_t* buf, size_t size) {
    int found = 0;
    for (size_t i = 0; i < size - 4; i++) {
        if (buf[i] == 0x56 && buf[i+1] == 0x4E) {
            uint16_t ver = buf[i+2] | (buf[i+3] << 8);
            if (ver == 2 || ver == 3) {
                found++;
                if (found <= 5) {
                    LOGI("  NV+ver%d at offset 0x%zx in payload", ver, i);
                }
            }
        }
    }
    return found;
}

void test_sb_fixed() {
    LOGI("test_sb_fixed: starting");
    
    const char* path = "/sdcard/Android/data/com.gameloft.android.ANMP.GloftN3HM/files/levels.gla";
    
    FILE* f = fopen(path, "rb");
    if (!f) {
        LOGE("test_sb_fixed: cannot open %s", path);
        return;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    uint8_t* buf = new uint8_t[size];
    fread(buf, 1, size, f);
    fclose(f);
    
    LOGI("test_sb_fixed: read %s (%ld bytes)", path, size);
    LOGI("test_sb_fixed: magic: %02x %02x", buf[0], buf[1]);
    
    if (buf[0] != 0x53 || buf[1] != 0x42) {
        LOGE("test_sb_fixed: not SB magic");
        delete[] buf;
        return;
    }
    
    // Estrutura SB (baseado no sb-format.md):
    // offset 0: magic "SB" (2 bytes)
    // offset 2: tag (2 bytes, little-endian)
    // offset 4: ?? (2 bytes?) 
    // offset 6: ?? (2 bytes?)
    // offset 8: nome XOR (6 bytes)
    // offset 14: payload
    
    // Vamos tentar ler o tamanho do payload do offset 4 (4 bytes)
    size_t pos = 0;
    int record_count = 0;
    int total_nv = 0;
    
    while (pos + 16 < size) {
        // Verificar magic
        if (buf[pos] != 0x53 || buf[pos+1] != 0x42) {
            pos += 1;
            continue;
        }
        
        uint16_t tag = buf[pos+2] | (buf[pos+3] << 8);
        
        // Tentar ler tamanho de diferentes offsets
        uint32_t size1 = buf[pos+4] | (buf[pos+5] << 8) | (buf[pos+6] << 16) | (buf[pos+7] << 24);
        uint16_t size2 = buf[pos+4] | (buf[pos+5] << 8);
        uint16_t size3 = buf[pos+6] | (buf[pos+7] << 8);
        
        // Nome XOR (6 bytes)
        uint8_t name[7] = {0};
        uint8_t mask[6] = {0x14, 0x00, 0x00, 0x00, 0x08, 0x00};
        for (int i = 0; i < 6; i++) {
            name[i] = buf[pos + 8 + i] ^ mask[i];
        }
        name[6] = '\0';
        
        LOGI("record %d: tag=0x%04x name='%s' size1=%u size2=%u size3=%u at 0x%zx", 
             record_count, tag, name, size1, size2, size3, pos);
        
        // Usar size1 se for razoável (< 1MB)
        uint32_t payload_size = 0;
        if (size1 > 0 && size1 < 1024*1024) {
            payload_size = size1;
        } else if (size2 > 0 && size2 < 1024*1024) {
            payload_size = size2;
        } else if (size3 > 0 && size3 < 1024*1024) {
            payload_size = size3;
        } else {
            // Heurística: payload termina antes do próximo magic SB
            // Procurar próximo "SB" no buffer
            size_t next_sb = pos + 14;
            while (next_sb + 2 < size) {
                if (buf[next_sb] == 0x53 && buf[next_sb+1] == 0x42) break;
                next_sb++;
            }
            if (next_sb > pos + 14) {
                payload_size = next_sb - (pos + 14);
                LOGI("  heuristic payload_size: %u", payload_size);
            } else {
                payload_size = 1024; // fallback
            }
        }
        
        if (payload_size > 0 && payload_size < size - pos) {
            size_t payload_start = pos + 14;
            uint8_t* payload = new uint8_t[payload_size];
            memcpy(payload, buf + payload_start, payload_size);
            
            // Aplicar TEA no payload
            decrypt_buffer(payload, payload_size);
            
            // Procurar NV magic
            int nv = find_nv_magic(payload, payload_size);
            if (nv > 0) {
                LOGI("  found %d NV+ver in payload of record %d", nv, record_count);
                total_nv += nv;
            }
            
            delete[] payload;
            pos += 14 + payload_size;
        } else {
            pos += 1024; // fallback
        }
        
        record_count++;
        if (record_count > 100) break;
    }
    
    LOGI("test_sb_fixed: total records: %d, total NV found: %d", record_count, total_nv);
    delete[] buf;
    LOGI("test_sb_fixed: done");
}
