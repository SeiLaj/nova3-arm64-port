#include <android/log.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>

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

static void tea_decrypt_buffer(uint8_t* buf, size_t size) {
    uint32_t k[4];
    get_tea_keys(k);
    uint32_t* data = (uint32_t*)buf;
    int blocks = size / 8;
    for (int i = 0; i < blocks; i++) {
        tea32_decrypt(&data[i*2], k);
    }
}

bool is_png(const uint8_t* data, size_t size) {
    return size > 8 && data[0] == 0x89 && data[1] == 0x50 && data[2] == 0x4E && data[3] == 0x47;
}

bool is_jpg(const uint8_t* data, size_t size) {
    return size > 2 && data[0] == 0xFF && data[1] == 0xD8;
}

bool is_dds(const uint8_t* data, size_t size) {
    return size > 4 && data[0] == 0x44 && data[1] == 0x44 && data[2] == 0x53 && data[3] == 0x20;
}

// Extrair do formato SBA (começa com 53 42 41)
std::vector<uint8_t> extract_from_sba(const char* path) {
    std::vector<uint8_t> result;
    
    FILE* f = fopen(path, "rb");
    if (!f) { LOGE("Cannot open: %s", path); return result; }
    
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    uint8_t* buf = new uint8_t[file_size];
    fread(buf, 1, file_size, f);
    fclose(f);
    
    LOGI("SBA file: %s (%ld bytes)", path, file_size);
    LOGI("Magic: %02x %02x %02x", buf[0], buf[1], buf[2]);
    
    if (buf[0] != 0x53 || buf[1] != 0x42 || buf[2] != 0x41) {
        LOGE("Not SBA magic!");
        delete[] buf;
        return result;
    }
    
    // Procurar padrões de imagem no buffer
    size_t pos = 0;
    int found = 0;
    
    while (pos + 16 < file_size && found < 10) {
        // Procurar por PNG magic
        if (buf[pos] == 0x89 && buf[pos+1] == 0x50 && buf[pos+2] == 0x4E && buf[pos+3] == 0x47) {
            LOGI("Found PNG at offset 0x%zx", pos);
            // Extrair o PNG (procurar pelo fim IEND)
            size_t end = pos + 8;
            while (end + 4 < file_size) {
                if (buf[end] == 0x49 && buf[end+1] == 0x45 && buf[end+2] == 0x4E && buf[end+3] == 0x44) {
                    size_t png_size = end + 8 - pos;
                    LOGI("  PNG size: %zu bytes", png_size);
                    result.resize(png_size);
                    memcpy(result.data(), buf + pos, png_size);
                    found++;
                    break;
                }
                end++;
            }
            pos = end;
        }
        // Procurar por JPG magic
        else if (buf[pos] == 0xFF && buf[pos+1] == 0xD8) {
            LOGI("Found JPG at offset 0x%zx", pos);
            // Extrair JPG (procurar pelo fim FFD9)
            size_t end = pos + 2;
            while (end + 2 < file_size) {
                if (buf[end] == 0xFF && buf[end+1] == 0xD9) {
                    size_t jpg_size = end + 2 - pos;
                    LOGI("  JPG size: %zu bytes", jpg_size);
                    result.resize(jpg_size);
                    memcpy(result.data(), buf + pos, jpg_size);
                    found++;
                    break;
                }
                end++;
            }
            pos = end;
        }
        else {
            pos++;
        }
    }
    
    LOGI("Found %d images in SBA", found);
    delete[] buf;
    return result;
}
