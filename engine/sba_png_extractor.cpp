#include <android/log.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>

#define LOG_TAG "NOVA3_engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

bool is_png(const uint8_t* data, size_t size) {
    return size > 8 && data[0] == 0x89 && data[1] == 0x50 && 
           data[2] == 0x4E && data[3] == 0x47;
}

std::vector<uint8_t> extract_first_png_from_sba(const char* path) {
    std::vector<uint8_t> result;
    
    FILE* f = fopen(path, "rb");
    if (!f) {
        LOGE("Cannot open: %s", path);
        return result;
    }
    
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    LOGI("SBA file: %s (%ld bytes)", path, file_size);
    
    uint8_t* buf = new uint8_t[file_size];
    size_t read = fread(buf, 1, file_size, f);
    fclose(f);
    
    LOGI("Read %zu bytes", read);
    
    // Procurar por magic PNG (89 50 4E 47)
    size_t pos = 0;
    int found = 0;
    
    while (pos + 8 < file_size && found < 3) {
        // Procurar o magic
        if (buf[pos] == 0x89 && buf[pos+1] == 0x50 && 
            buf[pos+2] == 0x4E && buf[pos+3] == 0x47) {
            
            LOGI("Found PNG at offset 0x%zx", pos);
            
            // Procurar pelo final do PNG (IEND chunk)
            size_t end = pos + 8;
            while (end + 12 < file_size) {
                if (buf[end] == 0x49 && buf[end+1] == 0x45 && 
                    buf[end+2] == 0x4E && buf[end+3] == 0x44) {
                    size_t png_size = end + 12 - pos;
                    LOGI("  PNG size: %zu bytes (IEND at 0x%zx)", png_size, end);
                    result.resize(png_size);
                    memcpy(result.data(), buf + pos, png_size);
                    found++;
                    break;
                }
                end++;
            }
            pos = end;
        } else {
            pos++;
        }
    }
    
    LOGI("Found %d PNG images", found);
    delete[] buf;
    return result;
}
