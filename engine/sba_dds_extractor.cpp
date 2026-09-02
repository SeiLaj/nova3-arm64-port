#include <android/log.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>

#define LOG_TAG "NOVA3_engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Detectar DDS pelo magic (44 44 53 20)
bool is_dds(const uint8_t* data, size_t size) {
    return size > 4 && data[0] == 0x44 && data[1] == 0x44 && 
           data[2] == 0x53 && data[3] == 0x20;
}

// Detectar PNG (fallback)
bool is_png(const uint8_t* data, size_t size) {
    return size > 8 && data[0] == 0x89 && data[1] == 0x50 && 
           data[2] == 0x4E && data[3] == 0x47;
}

// Extrair primeiro DDS ou PNG do SBA
std::vector<uint8_t> extract_first_image_from_sba(const char* path) {
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
    fread(buf, 1, file_size, f);
    fclose(f);
    
    size_t pos = 0;
    int found = 0;
    
    while (pos + 8 < file_size && found < 2) {
        // Procurar DDS magic
        if (buf[pos] == 0x44 && buf[pos+1] == 0x44 && 
            buf[pos+2] == 0x53 && buf[pos+3] == 0x20) {
            
            LOGI("Found DDS at offset 0x%zx", pos);
            
            // DDS header tem 124 bytes, seguido pelos dados
            // O tamanho total pode ser obtido do header
            size_t dds_start = pos;
            
            // Tentar encontrar o próximo DDS ou PNG para determinar o tamanho
            size_t next = pos + 128;
            while (next + 8 < file_size) {
                if ((buf[next] == 0x44 && buf[next+1] == 0x44 && 
                     buf[next+2] == 0x53 && buf[next+3] == 0x20) ||
                    (buf[next] == 0x89 && buf[next+1] == 0x50 && 
                     buf[next+2] == 0x4E && buf[next+3] == 0x47)) {
                    break;
                }
                next++;
            }
            
            size_t dds_size = next - dds_start;
            LOGI("  DDS size: %zu bytes", dds_size);
            result.resize(dds_size);
            memcpy(result.data(), buf + dds_start, dds_size);
            found++;
            break;
        }
        // Procurar PNG magic (fallback)
        else if (buf[pos] == 0x89 && buf[pos+1] == 0x50 && 
                 buf[pos+2] == 0x4E && buf[pos+3] == 0x47) {
            
            LOGI("Found PNG at offset 0x%zx", pos);
            
            size_t end = pos + 8;
            while (end + 12 < file_size) {
                if (buf[end] == 0x49 && buf[end+1] == 0x45 && 
                    buf[end+2] == 0x4E && buf[end+3] == 0x44) {
                    size_t png_size = end + 12 - pos;
                    LOGI("  PNG size: %zu bytes", png_size);
                    result.resize(png_size);
                    memcpy(result.data(), buf + pos, png_size);
                    found++;
                    break;
                }
                end++;
            }
            break;
        }
        pos++;
    }
    
    LOGI("Found %d image(s)", found);
    delete[] buf;
    return result;
}
