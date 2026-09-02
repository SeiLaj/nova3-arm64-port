#include <android/log.h>
#include <dlfcn.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sba_dds_extractor.cpp"


#include "third_party/stb_image.h"

#define LOG_TAG "NOVA3_engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// GLES functions
static void* g_gles = nullptr;
static bool g_renderer_init = false;
static unsigned int g_program = 0;
static unsigned int g_texture = 0;

typedef void (*glClearColor_t)(float, float, float, float);
typedef void (*glClear_t)(unsigned int);
typedef unsigned int (*glCreateShader_t)(unsigned int);
typedef void (*glShaderSource_t)(unsigned int, int, const char**, const int*);
typedef void (*glCompileShader_t)(unsigned int);
typedef unsigned int (*glCreateProgram_t)(void);
typedef void (*glAttachShader_t)(unsigned int, unsigned int);
typedef void (*glLinkProgram_t)(unsigned int);
typedef void (*glUseProgram_t)(unsigned int);
typedef int (*glGetUniformLocation_t)(unsigned int, const char*);
typedef void (*glUniform1i_t)(int, int);
typedef int (*glGetAttribLocation_t)(unsigned int, const char*);
typedef void (*glEnableVertexAttribArray_t)(unsigned int);
typedef void (*glDisableVertexAttribArray_t)(unsigned int);
typedef void (*glVertexAttribPointer_t)(unsigned int, int, unsigned int, unsigned char, int, const void*);
typedef void (*glDrawArrays_t)(unsigned int, int, int);
typedef void (*glGenTextures_t)(int, unsigned int*);
typedef void (*glBindTexture_t)(unsigned int, unsigned int);
typedef void (*glTexImage2D_t)(unsigned int, int, int, int, int, int, unsigned int, unsigned int, const void*);
typedef void (*glTexParameteri_t)(unsigned int, unsigned int, int);

static glClearColor_t p_glClearColor = nullptr;
static glClear_t p_glClear = nullptr;
static glCreateShader_t p_glCreateShader = nullptr;
static glShaderSource_t p_glShaderSource = nullptr;
static glCompileShader_t p_glCompileShader = nullptr;
static glCreateProgram_t p_glCreateProgram = nullptr;
static glAttachShader_t p_glAttachShader = nullptr;
static glLinkProgram_t p_glLinkProgram = nullptr;
static glUseProgram_t p_glUseProgram = nullptr;
static glGetUniformLocation_t p_glGetUniformLocation = nullptr;
static glUniform1i_t p_glUniform1i = nullptr;
static glGetAttribLocation_t p_glGetAttribLocation = nullptr;
static glEnableVertexAttribArray_t p_glEnableVertexAttribArray = nullptr;
static glDisableVertexAttribArray_t p_glDisableVertexAttribArray = nullptr;
static glVertexAttribPointer_t p_glVertexAttribPointer = nullptr;
static glDrawArrays_t p_glDrawArrays = nullptr;
static glGenTextures_t p_glGenTextures = nullptr;
static glBindTexture_t p_glBindTexture = nullptr;
static glTexImage2D_t p_glTexImage2D = nullptr;
static glTexParameteri_t p_glTexParameteri = nullptr;

static const char* vertex_shader =
    "attribute vec4 a_position;\n"
    "attribute vec2 a_texcoord;\n"
    "varying vec2 v_texcoord;\n"
    "void main() {\n"
    "  gl_Position = a_position;\n"
    "  v_texcoord = a_texcoord;\n"
    "}\n";

static const char* fragment_shader =
    "precision mediump float;\n"
    "varying vec2 v_texcoord;\n"
    "uniform sampler2D u_texture;\n"
    "void main() {\n"
    "  gl_FragColor = texture2D(u_texture, v_texcoord);\n"
    "}\n";

bool load_gles_functions() {
    const char* libs[] = {"libGLESv2.so", "/system/lib64/libGLESv2.so", "/vendor/lib64/libGLESv2.so"};
    for (int i = 0; i < 3; i++) {
        g_gles = dlopen(libs[i], RTLD_NOW);
        if (g_gles) { LOGI("GLES loaded: %s", libs[i]); break; }
    }
    if (!g_gles) { LOGE("Failed to load GLES"); return false; }
    
    p_glClearColor = (glClearColor_t)dlsym(g_gles, "glClearColor");
    p_glClear = (glClear_t)dlsym(g_gles, "glClear");
    p_glCreateShader = (glCreateShader_t)dlsym(g_gles, "glCreateShader");
    p_glShaderSource = (glShaderSource_t)dlsym(g_gles, "glShaderSource");
    p_glCompileShader = (glCompileShader_t)dlsym(g_gles, "glCompileShader");
    p_glCreateProgram = (glCreateProgram_t)dlsym(g_gles, "glCreateProgram");
    p_glAttachShader = (glAttachShader_t)dlsym(g_gles, "glAttachShader");
    p_glLinkProgram = (glLinkProgram_t)dlsym(g_gles, "glLinkProgram");
    p_glUseProgram = (glUseProgram_t)dlsym(g_gles, "glUseProgram");
    p_glGetUniformLocation = (glGetUniformLocation_t)dlsym(g_gles, "glGetUniformLocation");
    p_glUniform1i = (glUniform1i_t)dlsym(g_gles, "glUniform1i");
    p_glGetAttribLocation = (glGetAttribLocation_t)dlsym(g_gles, "glGetAttribLocation");
    p_glEnableVertexAttribArray = (glEnableVertexAttribArray_t)dlsym(g_gles, "glEnableVertexAttribArray");
    p_glDisableVertexAttribArray = (glDisableVertexAttribArray_t)dlsym(g_gles, "glDisableVertexAttribArray");
    p_glVertexAttribPointer = (glVertexAttribPointer_t)dlsym(g_gles, "glVertexAttribPointer");
    p_glDrawArrays = (glDrawArrays_t)dlsym(g_gles, "glDrawArrays");
    p_glGenTextures = (glGenTextures_t)dlsym(g_gles, "glGenTextures");
    p_glBindTexture = (glBindTexture_t)dlsym(g_gles, "glBindTexture");
    p_glTexImage2D = (glTexImage2D_t)dlsym(g_gles, "glTexImage2D");
    p_glTexParameteri = (glTexParameteri_t)dlsym(g_gles, "glTexParameteri");
    
    return true;
}

bool init_shaders() {
    unsigned int vs = p_glCreateShader(0x8B31);
    p_glShaderSource(vs, 1, &vertex_shader, nullptr);
    p_glCompileShader(vs);
    unsigned int fs = p_glCreateShader(0x8B30);
    p_glShaderSource(fs, 1, &fragment_shader, nullptr);
    p_glCompileShader(fs);
    g_program = p_glCreateProgram();
    p_glAttachShader(g_program, vs);
    p_glAttachShader(g_program, fs);
    p_glLinkProgram(g_program);
    LOGI("Shader program created");
    return true;
}

unsigned int create_test_texture() {
    int w = 128, h = 128;
    unsigned char data[128*128*4];
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y*w + x) * 4;
            int checker = ((x/16) + (y/16)) % 2;
            data[idx+0] = checker ? 255 : 200;
            data[idx+1] = checker ? 50 : 255;
            data[idx+2] = checker ? 255 : 50;
            data[idx+3] = 255;
        }
    }
    unsigned int tex;
    p_glGenTextures(1, &tex);
    p_glBindTexture(0x0DE1, tex);
    p_glTexImage2D(0x0DE1, 0, 0x1908, w, h, 0, 0x1908, 0x1401, data);
    p_glTexParameteri(0x0DE1, 0x2800, 0x2601);
    p_glTexParameteri(0x0DE1, 0x2801, 0x2601);
    LOGI("Fallback texture created");
    return tex;
}

bool init_renderer() {
    if (g_renderer_init) return true;
    if (!load_gles_functions()) return false;
    if (!init_shaders()) return false;
    
    // Tentar extrair PNG do actors.gla
    const char* path = "/sdcard/Android/data/com.gameloft.android.ANMP.GloftN3HM/files/actors.gla";
    LOGI("Extracting PNG from: %s", path);
    
    auto png_data = extract_first_image_from_sba(path);
    if (!png_data.empty()) {
        LOGI("Extracted PNG: %zu bytes", png_data.size());
        int w, h, comp;
        unsigned char* img = stbi_load_from_memory(png_data.data(), png_data.size(), &w, &h, &comp, 4);
        if (img) {
            LOGI("Decoded PNG: %dx%d, comp=%d", w, h, comp);
            p_glGenTextures(1, &g_texture);
            p_glBindTexture(0x0DE1, g_texture);
            p_glTexImage2D(0x0DE1, 0, 0x1908, w, h, 0, 0x1908, 0x1401, img);
            p_glTexParameteri(0x0DE1, 0x2800, 0x2601);
            p_glTexParameteri(0x0DE1, 0x2801, 0x2601);
            stbi_image_free(img);
            LOGI("SUCCESS: Texture loaded from actors.gla!");
        }
    }
    
    // Fallback
    if (!g_texture) {
        LOGI("No PNG found, using fallback");
        g_texture = create_test_texture();
    }
    
    g_renderer_init = true;
    return true;
}

void render_textured_quad() {
    if (!g_renderer_init) init_renderer();
    if (!g_texture) return;
    
    p_glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    p_glClear(0x00004000);
    
    p_glUseProgram(g_program);
    p_glUniform1i(p_glGetUniformLocation(g_program, "u_texture"), 0);
    p_glBindTexture(0x0DE1, g_texture);
    
    float vertices[] = {
        -0.9f, -0.9f, 0.0f,  0.0f, 0.0f,
         0.9f, -0.9f, 0.0f,  1.0f, 0.0f,
        -0.9f,  0.9f, 0.0f,  0.0f, 1.0f,
         0.9f,  0.9f, 0.0f,  1.0f, 1.0f,
    };
    
    int pos_loc = p_glGetAttribLocation(g_program, "a_position");
    int tex_loc = p_glGetAttribLocation(g_program, "a_texcoord");
    
    p_glEnableVertexAttribArray(pos_loc);
    p_glVertexAttribPointer(pos_loc, 3, 0x1406, 0, 20, vertices);
    p_glEnableVertexAttribArray(tex_loc);
    p_glVertexAttribPointer(tex_loc, 2, 0x1406, 0, 20, vertices + 3);
    p_glDrawArrays(0x0005, 0, 4);
    p_glDisableVertexAttribArray(pos_loc);
    p_glDisableVertexAttribArray(tex_loc);
}
