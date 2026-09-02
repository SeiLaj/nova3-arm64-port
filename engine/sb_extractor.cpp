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

// Detectar formatos de imagem
bool is_png(const uint8_t* data, size_t size) {
    if (size < 8) return false;
    return data[0] == 0x89 && data[1] == 0x50 && data[2] == 0x4E && data[3] == 0x47;
}

bool is_jpg(const uint8_t* data, size_t size) {
    if (size < 2) return false;
    return data[0] == 0xFF && data[1] == 0xD8;
}

bool is_dds(const uint8_t* data, size_t size) {
    if (size < 4) return false;
    return data[0] == 0x44 && data[1] == 0x44 && data[2] == 0x53 && data[3] == 0x20;
}

struct SBRecord {
    uint16_t tag;
    uint32_t size;
    std::string name;
    std::vector<uint8_t> data;
    bool is_image;
    std::string format; // "png", "jpg", "dds"
};

std::vector<SBRecord> extract_all_records(const char* path) {
    std::vector<SBRecord> records;
    
    FILE* f = fopen(path, "rb");
    if (!f) {
        LOGE("Cannot open: %s", path);
        return records;
    }
    
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    uint8_t* buf = new uint8_t[file_size];
    fread(buf, 1, file_size, f);
    fclose(f);
    
    LOGI("File: %s (%ld bytes)", path, file_size);
    
    if (file_size < 16 || buf[0] != 0x53 || buf[1] != 0x42) {
        LOGE("Not SB magic: %02x %02x", buf[0], buf[1]);
        delete[] buf;
        return records;
    }
    
    size_t pos = 0;
    int record_count = 0;
    
    while (pos + 16 < file_size) {
        // Procurar próximo magic SB
        if (buf[pos] != 0x53 || buf[pos+1] != 0x42) {
            pos++;
            continue;
        }
        
        SBRecord rec;
        rec.tag = buf[pos+2] | (buf[pos+3] << 8);
        rec.size = buf[pos+4] | (buf[pos+5] << 8) | (buf[pos+6] << 16) | (buf[pos+7] << 24);
        
        // Nome XOR (6 bytes)
        uint8_t mask[6] = {0x14, 0x00, 0x00, 0x00, 0x08, 0x00};
        for (int i = 0; i < 6; i++) {
            char c = buf[pos + 8 + i] ^ mask[i];
            if (c >= 0x20 && c <= 0x7E) rec.name += c;
        }
        
        size_t payload_start = pos + 14;
        
        if (rec.size > 0 && rec.size < file_size - payload_start) {
            rec.data.resize(rec.size);
            memcpy(rec.data.data(), buf + payload_start, rec.size);
            
            // Verificar se é uma imagem antes de decriptar
            rec.is_image = is_png(rec.data.data(), rec.size) || 
                          is_jpg(rec.data.data(), rec.size) ||
                          is_dds(rec.data.data(), rec.size);
            
            if (!rec.is_image) {
                // Tentar decriptar e verificar novamente
                tea_decrypt_buffer(rec.data.data(), rec.size);
                rec.is_image = is_png(rec.data.data(), rec.size) || 
                              is_jpg(rec.data.data(), rec.size) ||
                              is_dds(rec.data.data(), rec.size);
            }
            
            if (rec.is_image) {
                if (is_png(rec.data.data(), rec.size)) rec.format = "png";
                else if (is_jpg(rec.data.data(), rec.size)) rec.format = "jpg";
                else if (is_dds(rec.data.data(), rec.size)) rec.format = "dds";
                LOGI("Found image: tag=0x%04x name='%s' size=%u format=%s", 
                     rec.tag, rec.name.c_str(), rec.size, rec.format.c_str());
            }
            
            records.push_back(rec);
            record_count++;
        }
        
        pos += 14 + rec.size;
        if (record_count > 500) break;
    }
    
    LOGI("Total records: %d, images: %zu", record_count, 
         std::count_if(records.begin(), records.end(), [](auto& r){ return r.is_image; }));
    
    delete[] buf;
    return records;
}

// Encontrar a primeira imagem (prioridade PNG > JPG > DDS)
const SBRecord* find_first_image(const std::vector<SBRecord>& records) {
    // Primeiro PNG
    for (const auto& r : records) {
        if (r.is_image && r.format == "png") return &r;
    }
    // Depois JPG
    for (const auto& r : records) {
        if (r.is_image && r.format == "jpg") return &r;
    }
    // Depois DDS
    for (const auto& r : records) {
        if (r.is_image && r.format == "dds") return &r;
    }
    return nullptr;
}
