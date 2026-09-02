#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include <android/log.h>

#define LOG_TAG "TEST"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

bool is_png(const uint8_t* data, size_t size) {
    return size > 8 && data[0] == 0x89 && data[1] == 0x50 && 
           data[2] == 0x4E && data[3] == 0x47;
}

int main() {
    const char* path = "/sdcard/Android/data/com.gameloft.android.ANMP.GloftN3HM/files/actors.gla";
    LOGI("Testing: %s", path);
    
    FILE* f = fopen(path, "rb");
    if (!f) {
        LOGI("Cannot open file!");
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    LOGI("File size: %ld bytes", size);
    
    uint8_t* buf = new uint8_t[size];
    fread(buf, 1, size, f);
    fclose(f);
    
    LOGI("First 16 bytes: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
         buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
         buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
    
    // Procurar PNG
    int found = 0;
    for (size_t i = 0; i < size - 8 && found < 3; i++) {
        if (buf[i] == 0x89 && buf[i+1] == 0x50 && buf[i+2] == 0x4E && buf[i+3] == 0x47) {
            LOGI("Found PNG at offset 0x%zx", i);
            found++;
        }
    }
    LOGI("Total PNG found: %d", found);
    
    delete[] buf;
    return 0;
}
