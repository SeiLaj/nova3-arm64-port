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
                if (found <= 3) {
                    LOGI("  NV+ver%d at offset 0x%zx", ver, i);
                }
            }
        }
    }
    return found;
}

void process_sb_file(const char* path) {
    LOGI("process_sb_file: %s", path);
    
    FILE* f = fopen(path, "rb");
    if (!f) {
        LOGE("cannot open %s", path);
        return;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    uint8_t* buf = new uint8_t[size];
    fread(buf, 1, size, f);
    fclose(f);
    
    if (buf[0] != 0x53 || buf[1] != 0x42) {
        LOGI("not SB magic, skipping");
        delete[] buf;
        return;
    }
    
    int total_nv = 0;
    int record_count = 0;
    size_t pos = 0;
    
    while (pos + 16 < size) {
        if (buf[pos] != 0x53 || buf[pos+1] != 0x42) {
            pos++;
            continue;
        }
        
        uint16_t tag = buf[pos+2] | (buf[pos+3] << 8);
        uint32_t payload_size = buf[pos+4] | (buf[pos+5] << 8) | (buf[pos+6] << 16) | (buf[pos+7] << 24);
        
        if (payload_size > 0 && payload_size < 1024*1024) {
            size_t payload_start = pos + 14;
            if (payload_start + payload_size <= size) {
                uint8_t* payload = new uint8_t[payload_size];
                memcpy(payload, buf + payload_start, payload_size);
                decrypt_buffer(payload, payload_size);
                int nv = find_nv_magic(payload, payload_size);
                if (nv > 0) total_nv += nv;
                delete[] payload;
            }
            pos += 14 + payload_size;
        } else {
            pos += 1024;
        }
        record_count++;
        if (record_count > 100) break;
    }
    
    LOGI("  records: %d, NV found: %d", record_count, total_nv);
    delete[] buf;
}

void test_all_files() {
    LOGI("test_all_files: starting");
    
    const char* files[] = {
        "levels.gla",
        "actors.gla",
        "weapons.gla",
        "sprites.gla",
        "sounds_hi.gla",
        "effects.gla",
        "menus.gla"
    };
    
    for (int i = 0; i < 7; i++) {
        char path[256];
        snprintf(path, sizeof(path), "/sdcard/Android/data/com.gameloft.android.ANMP.GloftN3HM/files/%s", files[i]);
        process_sb_file(path);
    }
    
    LOGI("test_all_files: done");
}
