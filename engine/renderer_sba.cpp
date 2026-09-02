#include <android/log.h>
#include <dlfcn.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sba_extractor.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

// ... (GLES functions - mesmo código do renderizador anterior, omitido por brevidade)

// [TODO: Copiar todas as GLES functions do renderizador_texture.cpp aqui]

bool init_renderer() {
    if (g_renderer_init) return true;
    if (!load_gles_functions()) return false;
    if (!init_shaders()) return false;
    
    // Tentar extrair imagem do actors.gla (SBA)
    const char* path = "/sdcard/Android/data/com.gameloft.android.ANMP.GloftN3HM/files/actors.gla";
    LOGI("Scanning SBA: %s", path);
    
    auto img_data = extract_from_sba(path);
    if (!img_data.empty()) {
        LOGI("Extracted %zu bytes from SBA", img_data.size());
        int w, h, comp;
        unsigned char* img = stbi_load_from_memory(img_data.data(), img_data.size(), &w, &h, &comp, 4);
        if (img) {
            LOGI("Loaded image: %dx%d, %d channels", w, h, comp);
            p_glGenTextures(1, &g_texture);
            p_glBindTexture(0x0DE1, g_texture);
            p_glTexImage2D(0x0DE1, 0, 0x1908, w, h, 0, 0x1908, 0x1401, img);
            p_glTexParameteri(0x0DE1, 0x2800, 0x2601);
            p_glTexParameteri(0x0DE1, 0x2801, 0x2601);
            stbi_image_free(img);
            LOGI("Texture loaded from SBA!");
        }
    }
    
    // Fallback: xadrez
    if (!g_texture) {
        LOGI("No image found in SBA, using fallback");
        g_texture = create_test_texture();
    }
    
    g_renderer_init = true;
    return true;
}
