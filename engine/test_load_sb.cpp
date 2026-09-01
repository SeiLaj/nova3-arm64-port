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

bool process_sb_file(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        LOGE("process_sb: cannot open %s", path);
        return false;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    if (size < 16) {
        LOGE("process_sb: file too small");
        fclose(f);
        return false;
    }
    
    uint8_t* buf = new uint8_t[size];
    fread(buf, 1, size, f);
    fclose(f);
    
    LOGI("process_sb: read %s (%ld bytes)", path, size);
    
    // Ofuscar keys (ROR11 XOR 0xF3042911)
    uint32_t k[4];
    for (int i = 0; i < 4; i++) {
        uint32_t raw = TEA_KEYS[i];
        uint32_t x = raw ^ 0xF3042911;
        k[i] = (x >> 11) | (x << (32 - 11));
    }
    
    // Aplicar TEA em todo o buffer (blocos de 8 bytes)
    uint32_t* data = (uint32_t*)buf;
    int blocks = size / 8;
    for (int i = 0; i < blocks; i++) {
        tea32_decrypt(&data[i*2], k);
    }
    
    // Procurar magic 0x4E56 (NV) + versão 2 ou 3
    int found = 0;
    for (int i = 0; i < size - 4; i++) {
        if (buf[i] == 0x56 && buf[i+1] == 0x4E) {
            uint16_t ver = buf[i+2] | (buf[i+3] << 8);
            if (ver == 2 || ver == 3) {
                found++;
                LOGI("process_sb: found NV+ver%d at offset %d (0x%x)", ver, i, i);
                if (found <= 3) {
                    // Mostrar contexto
                    LOGI("  bytes at offset: %02x %02x %02x %02x %02x %02x %02x %02x",
                         buf[i], buf[i+1], buf[i+2], buf[i+3],
                         buf[i+4], buf[i+5], buf[i+6], buf[i+7]);
                }
            }
        }
    }
    
    LOGI("process_sb: total NV+ver2/3 found: %d", found);
    delete[] buf;
    return true;
}

void test_sb_processing() {
    LOGI("test_sb: starting");
    
    const char* paths[] = {
        "/sdcard/Android/data/com.gameloft.android.ANMP.GloftN3HM/files/menus4.gla",
        "/sdcard/Android/data/com.gameloft.android.ANMP.GloftN3HM/files/menus.gla",
    };
    
    for (int i = 0; i < 2; i++) {
        if (access(paths[i], F_OK) == 0) {
            LOGI("test_sb: processing %s", paths[i]);
            process_sb_file(paths[i]);
        } else {
            LOGI("test_sb: file NOT found: %s", paths[i]);
        }
    }
    
    LOGI("test_sb: done");
}
