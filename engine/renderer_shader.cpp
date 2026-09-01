#include <android/log.h>
#include <dlfcn.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define LOG_TAG "NOVA3_engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static void* g_gles = nullptr;
static bool g_renderer_init = false;
static unsigned int g_program = 0;

typedef void (*glClearColor_t)(float, float, float, float);
typedef void (*glClear_t)(unsigned int);
typedef void (*glViewport_t)(int, int, int, int);
typedef unsigned int (*glCreateShader_t)(unsigned int);
typedef void (*glShaderSource_t)(unsigned int, int, const char**, const int*);
typedef void (*glCompileShader_t)(unsigned int);
typedef unsigned int (*glCreateProgram_t)(void);
typedef void (*glAttachShader_t)(unsigned int, unsigned int);
typedef void (*glLinkProgram_t)(unsigned int);
typedef void (*glUseProgram_t)(unsigned int);
typedef int (*glGetUniformLocation_t)(unsigned int, const char*);
typedef void (*glUniform4f_t)(int, float, float, float, float);
typedef int (*glGetAttribLocation_t)(unsigned int, const char*);
typedef void (*glEnableVertexAttribArray_t)(unsigned int);
typedef void (*glDisableVertexAttribArray_t)(unsigned int);
typedef void (*glVertexAttribPointer_t)(unsigned int, int, unsigned int, unsigned char, int, const void*);
typedef void (*glDrawArrays_t)(unsigned int, int, int);

static glClearColor_t p_glClearColor = nullptr;
static glClear_t p_glClear = nullptr;
static glViewport_t p_glViewport = nullptr;
static glCreateShader_t p_glCreateShader = nullptr;
static glShaderSource_t p_glShaderSource = nullptr;
static glCompileShader_t p_glCompileShader = nullptr;
static glCreateProgram_t p_glCreateProgram = nullptr;
static glAttachShader_t p_glAttachShader = nullptr;
static glLinkProgram_t p_glLinkProgram = nullptr;
static glUseProgram_t p_glUseProgram = nullptr;
static glGetUniformLocation_t p_glGetUniformLocation = nullptr;
static glUniform4f_t p_glUniform4f = nullptr;
static glGetAttribLocation_t p_glGetAttribLocation = nullptr;
static glEnableVertexAttribArray_t p_glEnableVertexAttribArray = nullptr;
static glDisableVertexAttribArray_t p_glDisableVertexAttribArray = nullptr;
static glVertexAttribPointer_t p_glVertexAttribPointer = nullptr;
static glDrawArrays_t p_glDrawArrays = nullptr;

static const char* vertex_shader =
    "attribute vec4 a_position;\n"
    "void main() {\n"
    "  gl_Position = a_position;\n"
    "}\n";

static const char* fragment_shader =
    "precision mediump float;\n"
    "uniform vec4 u_color;\n"
    "void main() {\n"
    "  gl_FragColor = u_color;\n"
    "}\n";

bool load_gles_functions() {
    const char* libs[] = {
        "libGLESv2.so",
        "/system/lib64/libGLESv2.so",
        "/vendor/lib64/libGLESv2.so",
        "/system/lib/libGLESv2.so"
    };
    
    for (int i = 0; i < 4; i++) {
        g_gles = dlopen(libs[i], RTLD_NOW);
        if (g_gles) {
            LOGI("GLES loaded: %s", libs[i]);
            break;
        }
    }
    
    if (!g_gles) {
        LOGE("Failed to load GLES library");
        return false;
    }
    
    p_glClearColor = (glClearColor_t)dlsym(g_gles, "glClearColor");
    p_glClear = (glClear_t)dlsym(g_gles, "glClear");
    p_glViewport = (glViewport_t)dlsym(g_gles, "glViewport");
    p_glCreateShader = (glCreateShader_t)dlsym(g_gles, "glCreateShader");
    p_glShaderSource = (glShaderSource_t)dlsym(g_gles, "glShaderSource");
    p_glCompileShader = (glCompileShader_t)dlsym(g_gles, "glCompileShader");
    p_glCreateProgram = (glCreateProgram_t)dlsym(g_gles, "glCreateProgram");
    p_glAttachShader = (glAttachShader_t)dlsym(g_gles, "glAttachShader");
    p_glLinkProgram = (glLinkProgram_t)dlsym(g_gles, "glLinkProgram");
    p_glUseProgram = (glUseProgram_t)dlsym(g_gles, "glUseProgram");
    p_glGetUniformLocation = (glGetUniformLocation_t)dlsym(g_gles, "glGetUniformLocation");
    p_glUniform4f = (glUniform4f_t)dlsym(g_gles, "glUniform4f");
    p_glGetAttribLocation = (glGetAttribLocation_t)dlsym(g_gles, "glGetAttribLocation");
    p_glEnableVertexAttribArray = (glEnableVertexAttribArray_t)dlsym(g_gles, "glEnableVertexAttribArray");
    p_glDisableVertexAttribArray = (glDisableVertexAttribArray_t)dlsym(g_gles, "glDisableVertexAttribArray");
    p_glVertexAttribPointer = (glVertexAttribPointer_t)dlsym(g_gles, "glVertexAttribPointer");
    p_glDrawArrays = (glDrawArrays_t)dlsym(g_gles, "glDrawArrays");
    
    if (!p_glClearColor || !p_glClear || !p_glViewport ||
        !p_glCreateShader || !p_glShaderSource || !p_glCompileShader ||
        !p_glCreateProgram || !p_glAttachShader || !p_glLinkProgram ||
        !p_glUseProgram || !p_glGetUniformLocation || !p_glUniform4f ||
        !p_glGetAttribLocation || !p_glEnableVertexAttribArray ||
        !p_glDisableVertexAttribArray || !p_glVertexAttribPointer || !p_glDrawArrays) {
        LOGE("Failed to load GLES functions");
        return false;
    }
    
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
    
    LOGI("Shader program created: %u", g_program);
    return true;
}

bool init_renderer() {
    if (g_renderer_init) return true;
    
    if (!load_gles_functions()) {
        return false;
    }
    
    if (!init_shaders()) {
        return false;
    }
    
    g_renderer_init = true;
    LOGI("Renderer initialized with shaders");
    return true;
}

void render_quad() {
    if (!g_renderer_init) {
        if (!init_renderer()) return;
    }
    
    p_glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    p_glClear(0x00004000);
    
    p_glUseProgram(g_program);
    
    int color_loc = p_glGetUniformLocation(g_program, "u_color");
    p_glUniform4f(color_loc, 1.0f, 1.0f, 1.0f, 1.0f);
    
    float vertices[] = {
        -0.3f, -0.3f, 0.0f,
         0.3f, -0.3f, 0.0f,
        -0.3f,  0.3f, 0.0f,
         0.3f,  0.3f, 0.0f,
    };
    
    int pos_loc = p_glGetAttribLocation(g_program, "a_position");
    p_glEnableVertexAttribArray(pos_loc);
    p_glVertexAttribPointer(pos_loc, 3, 0x1406, 0, 0, vertices);
    p_glDrawArrays(0x0005, 0, 4);
    p_glDisableVertexAttribArray(pos_loc);
}
