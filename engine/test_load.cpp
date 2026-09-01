#include <android/log.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>

#define LOG_TAG "NOVA3_engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// TEA-32 (já implementado em nova3_io.cpp)
extern void tea32_decrypt(uint32_t *v, const uint32_t k[4]);

// Subkeys do CEncryption (documentadas)
static const uint32_t TEA_KEYS[4] = {
    0x453465F2, 0x453477F2, 0x466465F2, 0x453433F2
};

bool test_decrypt_file(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        LOGE("test_load: cannot open %s", path);
        return false;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    if (size < 16) {
        LOGE("test_load: file too small");
        fclose(f);
        return false;
    }
    
    uint8_t* buf = new uint8_t[size];
    fread(buf, 1, size, f);
    fclose(f);
    
    LOGI("test_load: read %s (%ld bytes)", path, size);
    
    // Mostrar primeiros 16 bytes (antes do decrypt)
    LOGI("test_load: raw first 16: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
         buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],
         buf[8],buf[9],buf[10],buf[11],buf[12],buf[13],buf[14],buf[15]);
    
    // Tentar decrypt TEA no primeiro bloco (8 bytes)
    uint32_t block[2];
    memcpy(block, buf, 8);
    
    // Ofuscar keys (ROR11 XOR 0xF3042911)
    uint32_t k[4];
    for (int i = 0; i < 4; i++) {
        uint32_t raw = TEA_KEYS[i];
        uint32_t x = raw ^ 0xF3042911;
        // ROR11
        k[i] = (x >> 11) | (x << (32 - 11));
    }
    
    LOGI("test_load: keys after obfuscation: %08x %08x %08x %08x", k[0], k[1], k[2], k[3]);
    
    tea32_decrypt(block, k);
    
    LOGI("test_load: decrypted block: %08x %08x", block[0], block[1]);
    
    delete[] buf;
    return true;
}

// Função para ser chamada pelo App::MyInit()
void test_load_files() {
    LOGI("test_load: starting");
    
    const char* paths[] = {
        "/sdcard/Android/data/com.gameloft.android.ANMP.GloftN3HM/files/menus4.gla",
        "/sdcard/Android/data/com.gameloft.android.ANMP.GloftN3HM/files/levels.gla",
        "/sdcard/Android/obb/com.gameloft.android.ANMP.GloftN3HM/main.1050.com.gameloft.android.ANMP.GloftN3HM.obb"
    };
    
    for (int i = 0; i < 3; i++) {
        if (access(paths[i], F_OK) == 0) {
            LOGI("test_load: file exists: %s", paths[i]);
            test_decrypt_file(paths[i]);
        } else {
            LOGI("test_load: file NOT found: %s", paths[i]);
        }
    }
    
    LOGI("test_load: done");
}
