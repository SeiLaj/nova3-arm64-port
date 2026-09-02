#include <android/log.h>
#include <dlfcn.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define LOG_TAG "NOVA3_engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ========== GLES TYPEDEFS ==========
typedef void (*glClearColor_t)(float, float, float, float);
typedef void (*glClear_t)(unsigned int);
typedef void (*glGenTextures_t)(int, unsigned int*);
typedef void (*glBindTexture_t)(unsigned int, unsigned int);
typedef void (*glTexParameteri_t)(unsigned int, unsigned int, int);
typedef void (*glTexImage2D_t)(unsigned int, int, int, int, int, int, unsigned int, unsigned int, const void*);
typedef void (*glCompressedTexImage2D_t)(unsigned int, int, unsigned int, int, int, int, int, const void*);
typedef void (*glGenBuffers_t)(int, unsigned int*);
typedef void (*glBindBuffer_t)(unsigned int, unsigned int);
typedef void (*glBufferData_t)(unsigned int, long, const void*, unsigned int);
typedef unsigned int (*glCreateShader_t)(unsigned int);
typedef void (*glShaderSource_t)(unsigned int, int, const char**, const int*);
typedef void (*glCompileShader_t)(unsigned int);
typedef void (*glGetShaderiv_t)(unsigned int, unsigned int, int*);
typedef void (*glGetShaderInfoLog_t)(unsigned int, int, int*, char*);
typedef unsigned int (*glCreateProgram_t)(void);
typedef void (*glAttachShader_t)(unsigned int, unsigned int);
typedef void (*glLinkProgram_t)(unsigned int);
typedef void (*glGetProgramiv_t)(unsigned int, unsigned int, int*);
typedef void (*glGetProgramInfoLog_t)(unsigned int, int, int*, char*);
typedef void (*glUseProgram_t)(unsigned int);
typedef int (*glGetUniformLocation_t)(unsigned int, const char*);
typedef void (*glUniform1i_t)(int, int);
typedef int (*glGetAttribLocation_t)(unsigned int, const char*);
typedef void (*glEnableVertexAttribArray_t)(unsigned int);
typedef void (*glDisableVertexAttribArray_t)(unsigned int);
typedef void (*glVertexAttribPointer_t)(unsigned int, int, unsigned int, unsigned char, int, const void*);
typedef void (*glDrawArrays_t)(unsigned int, int, int);
typedef void (*glActiveTexture_t)(unsigned int);

// ========== PONTEIROS ==========
static void* g_gles = nullptr;
static glClearColor_t p_glClearColor = nullptr;
static glClear_t p_glClear = nullptr;
static glGenTextures_t p_glGenTextures = nullptr;
static glBindTexture_t p_glBindTexture = nullptr;
static glTexParameteri_t p_glTexParameteri = nullptr;
static glTexImage2D_t p_glTexImage2D = nullptr;
static glCompressedTexImage2D_t p_glCompressedTexImage2D = nullptr;
static glGenBuffers_t p_glGenBuffers = nullptr;
static glBindBuffer_t p_glBindBuffer = nullptr;
static glBufferData_t p_glBufferData = nullptr;
static glCreateShader_t p_glCreateShader = nullptr;
static glShaderSource_t p_glShaderSource = nullptr;
static glCompileShader_t p_glCompileShader = nullptr;
static glGetShaderiv_t p_glGetShaderiv = nullptr;
static glGetShaderInfoLog_t p_glGetShaderInfoLog = nullptr;
static glCreateProgram_t p_glCreateProgram = nullptr;
static glAttachShader_t p_glAttachShader = nullptr;
static glLinkProgram_t p_glLinkProgram = nullptr;
static glGetProgramiv_t p_glGetProgramiv = nullptr;
static glGetProgramInfoLog_t p_glGetProgramInfoLog = nullptr;
static glUseProgram_t p_glUseProgram = nullptr;
static glGetUniformLocation_t p_glGetUniformLocation = nullptr;
static glUniform1i_t p_glUniform1i = nullptr;
static glGetAttribLocation_t p_glGetAttribLocation = nullptr;
static glEnableVertexAttribArray_t p_glEnableVertexAttribArray = nullptr;
static glDisableVertexAttribArray_t p_glDisableVertexAttribArray = nullptr;
static glVertexAttribPointer_t p_glVertexAttribPointer = nullptr;
static glDrawArrays_t p_glDrawArrays = nullptr;
static glActiveTexture_t p_glActiveTexture = nullptr;

// ========== ESTADO ==========
static unsigned int g_program = 0;
static unsigned int g_texture = 0;
static unsigned int g_vbo = 0;
static int g_tex_w = 0, g_tex_h = 0;
static bool g_ready = false;

static const char* vertex_shader =
    "attribute vec4 aPos;\n"
    "attribute vec2 aTex;\n"
    "varying vec2 vTex;\n"
    "void main() {\n"
    "  gl_Position = aPos;\n"
    "  vTex = aTex;\n"
    "}\n";

static const char* fragment_shader =
    "precision mediump float;\n"
    "varying vec2 vTex;\n"
    "uniform sampler2D uTex;\n"
    "void main() {\n"
    "  gl_FragColor = texture2D(uTex, vTex);\n"
    "}\n";

// ========== CARREGAR GLES ==========
static bool load_gles() {
    if (g_gles) return true;
    const char* libs[] = {"libGLESv2.so", "/system/lib64/libGLESv2.so", nullptr};
    for (int i = 0; libs[i]; i++) {
        g_gles = dlopen(libs[i], RTLD_NOW);
        if (g_gles) { LOGI("GLES loaded: %s", libs[i]); break; }
    }
    if (!g_gles) { LOGE("Failed to load GLES"); return false; }
    
    p_glClearColor = (glClearColor_t)dlsym(g_gles, "glClearColor");
    p_glClear = (glClear_t)dlsym(g_gles, "glClear");
    p_glGenTextures = (glGenTextures_t)dlsym(g_gles, "glGenTextures");
    p_glBindTexture = (glBindTexture_t)dlsym(g_gles, "glBindTexture");
    p_glTexParameteri = (glTexParameteri_t)dlsym(g_gles, "glTexParameteri");
    p_glTexImage2D = (glTexImage2D_t)dlsym(g_gles, "glTexImage2D");
    p_glCompressedTexImage2D = (glCompressedTexImage2D_t)dlsym(g_gles, "glCompressedTexImage2D");
    p_glGenBuffers = (glGenBuffers_t)dlsym(g_gles, "glGenBuffers");
    p_glBindBuffer = (glBindBuffer_t)dlsym(g_gles, "glBindBuffer");
    p_glBufferData = (glBufferData_t)dlsym(g_gles, "glBufferData");
    p_glCreateShader = (glCreateShader_t)dlsym(g_gles, "glCreateShader");
    p_glShaderSource = (glShaderSource_t)dlsym(g_gles, "glShaderSource");
    p_glCompileShader = (glCompileShader_t)dlsym(g_gles, "glCompileShader");
    p_glGetShaderiv = (glGetShaderiv_t)dlsym(g_gles, "glGetShaderiv");
    p_glGetShaderInfoLog = (glGetShaderInfoLog_t)dlsym(g_gles, "glGetShaderInfoLog");
    p_glCreateProgram = (glCreateProgram_t)dlsym(g_gles, "glCreateProgram");
    p_glAttachShader = (glAttachShader_t)dlsym(g_gles, "glAttachShader");
    p_glLinkProgram = (glLinkProgram_t)dlsym(g_gles, "glLinkProgram");
    p_glGetProgramiv = (glGetProgramiv_t)dlsym(g_gles, "glGetProgramiv");
    p_glGetProgramInfoLog = (glGetProgramInfoLog_t)dlsym(g_gles, "glGetProgramInfoLog");
    p_glUseProgram = (glUseProgram_t)dlsym(g_gles, "glUseProgram");
    p_glGetUniformLocation = (glGetUniformLocation_t)dlsym(g_gles, "glGetUniformLocation");
    p_glUniform1i = (glUniform1i_t)dlsym(g_gles, "glUniform1i");
    p_glGetAttribLocation = (glGetAttribLocation_t)dlsym(g_gles, "glGetAttribLocation");
    p_glEnableVertexAttribArray = (glEnableVertexAttribArray_t)dlsym(g_gles, "glEnableVertexAttribArray");
    p_glDisableVertexAttribArray = (glDisableVertexAttribArray_t)dlsym(g_gles, "glDisableVertexAttribArray");
    p_glVertexAttribPointer = (glVertexAttribPointer_t)dlsym(g_gles, "glVertexAttribPointer");
    p_glDrawArrays = (glDrawArrays_t)dlsym(g_gles, "glDrawArrays");
    p_glActiveTexture = (glActiveTexture_t)dlsym(g_gles, "glActiveTexture");
    return true;
}

// ========== SHADERS ==========
static unsigned int compile_shader(unsigned int type, const char* src) {
    unsigned int sh = p_glCreateShader(type);
    p_glShaderSource(sh, 1, &src, nullptr);
    p_glCompileShader(sh);
    int ok;
    p_glGetShaderiv(sh, 0x8B81, &ok);
    if (!ok) {
        char log[512];
        p_glGetShaderInfoLog(sh, sizeof(log), nullptr, log);
        LOGE("Shader error: %s", log);
        return 0;
    }
    return sh;
}

static bool init_program() {
    unsigned int vs = compile_shader(0x8B31, vertex_shader);
    if (!vs) return false;
    unsigned int fs = compile_shader(0x8B30, fragment_shader);
    if (!fs) return false;
    g_program = p_glCreateProgram();
    p_glAttachShader(g_program, vs);
    p_glAttachShader(g_program, fs);
    p_glLinkProgram(g_program);
    int ok;
    p_glGetProgramiv(g_program, 0x8B82, &ok);
    if (!ok) {
        char log[512];
        p_glGetProgramInfoLog(g_program, sizeof(log), nullptr, log);
        LOGE("Program link error: %s", log);
        return false;
    }
    LOGI("Shader program OK: %u", g_program);
    return true;
}

// ========== CRIAR TEXTURA XADREZ (FALLBACK) ==========
static void create_checker_texture() {
    int w = 128, h = 128;
    unsigned char* data = (unsigned char*)malloc(w * h * 4);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int i = (y * w + x) * 4;
            int c = ((x / 16) + (y / 16)) % 2;
            data[i+0] = c ? 255 : 40;
            data[i+1] = c ? 40 : 255;
            data[i+2] = c ? 200 : 40;
            data[i+3] = 255;
        }
    }
    p_glGenTextures(1, &g_texture);
    p_glBindTexture(0x0DE1, g_texture);
    p_glTexParameteri(0x0DE1, 0x2801, 0x2601);
    p_glTexParameteri(0x0DE1, 0x2800, 0x2601);
    p_glTexImage2D(0x0DE1, 0, 0x1908, w, h, 0, 0x1908, 0x1401, data);
    free(data);
    g_tex_w = w; g_tex_h = h;
    LOGI("Checker texture created (fallback)");
}

// ========== TEXTURA ETC1 ==========
static bool load_etc1(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) { 
        LOGE("ETC1: fopen failed %s", path);
        create_checker_texture();
        return true; // fallback
    }
    unsigned char hdr[52];
    if (fread(hdr, 1, 52, f) != 52) { 
        fclose(f);
        create_checker_texture();
        return true;
    }
    int w = *(int*)(hdr + 4);
    int h = *(int*)(hdr + 8);
    fseek(f, 0, SEEK_END);
    long sz = ftell(f) - 52;
    fseek(f, 52, SEEK_SET);
    unsigned char* data = (unsigned char*)malloc(sz);
    if (!data) { fclose(f); create_checker_texture(); return true; }
    fread(data, 1, sz, f);
    fclose(f);
    LOGI("ETC1: %s %dx%d data=%ld", path, w, h, sz);

    p_glGenTextures(1, &g_texture);
    p_glBindTexture(0x0DE1, g_texture);
    p_glTexParameteri(0x0DE1, 0x2801, 0x2601);
    p_glTexParameteri(0x0DE1, 0x2800, 0x2601);
    p_glCompressedTexImage2D(0x0DE1, 0, 0x8D64, w, h, 0, sz, data);
    free(data);
    g_tex_w = w; g_tex_h = h;
    LOGI("ETC1 texture loaded: %u", g_texture);
    return true;
}

// ========== VBO ==========
static void init_vbo() {
    float verts[] = {
        -0.9f, -0.9f, 0.0f,  0.0f, 0.0f,
         0.9f, -0.9f, 0.0f,  1.0f, 0.0f,
        -0.9f,  0.9f, 0.0f,  0.0f, 1.0f,
         0.9f,  0.9f, 0.0f,  1.0f, 1.0f,
    };
    p_glGenBuffers(1, &g_vbo);
    p_glBindBuffer(0x8892, g_vbo);
    p_glBufferData(0x8892, sizeof(verts), verts, 0x88E4);
    LOGI("VBO created: %u", g_vbo);
}

// ========== EXPORTS ==========
extern "C" bool renderer_init(const char* etc_path) {
    if (g_ready) return true;
    if (!load_gles()) return false;
    if (!load_etc1(etc_path)) return false;
    if (!init_program()) return false;
    init_vbo();
    g_ready = true;
    LOGI("Renderer ready: tex=%u %dx%d", g_texture, g_tex_w, g_tex_h);
    return true;
}

extern "C" void renderer_draw() {
    LOGI("=== renderer_draw CHAMADO ===");
    LOGI("renderer_draw: g_ready=%d", g_ready); if (!g_ready) return;
    p_glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    p_glClear(0x00004000);

    p_glUseProgram(g_program);
    p_glActiveTexture(0x84C0);
    p_glBindTexture(0x0DE1, g_texture);
    int tex_loc = p_glGetUniformLocation(g_program, "uTex");
    p_glUniform1i(tex_loc, 0);

    p_glBindBuffer(0x8892, g_vbo);
    int pos_loc = p_glGetAttribLocation(g_program, "aPos");
    p_glEnableVertexAttribArray(pos_loc);
    p_glVertexAttribPointer(pos_loc, 3, 0x1406, 0, 5*sizeof(float), (void*)0);

    int tex_loc2 = p_glGetAttribLocation(g_program, "aTex");
    p_glEnableVertexAttribArray(tex_loc2);
    p_glVertexAttribPointer(tex_loc2, 2, 0x1406, 0, 5*sizeof(float), (void*)(3*sizeof(float)));

    p_glDrawArrays(0x0005, 0, 4);
    p_glDisableVertexAttribArray(pos_loc);
    p_glDisableVertexAttribArray(tex_loc2);
}
