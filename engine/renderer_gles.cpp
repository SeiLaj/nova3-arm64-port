#include <android/log.h>
#include <dlfcn.h>
#include <stdint.h>
#include <string.h>

#define LOG_TAG "NOVA3_engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Ponteiros para funções GLES
static void* g_gles = nullptr;
static bool g_renderer_init = false;

// Typedefs
typedef void (*glClearColor_t)(float, float, float, float);
typedef void (*glClear_t)(unsigned int);
typedef void (*glViewport_t)(int, int, int, int);

static glClearColor_t p_glClearColor = nullptr;
static glClear_t p_glClear = nullptr;
static glViewport_t p_glViewport = nullptr;

bool init_gles() {
    if (g_renderer_init) return true;
    
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
    
    if (!p_glClearColor || !p_glClear || !p_glViewport) {
        LOGE("Failed to load GLES functions");
        return false;
    }
    
    g_renderer_init = true;
    LOGI("GLES renderer initialized");
    return true;
}

void render_frame() {
    if (!g_renderer_init) {
        if (!init_gles()) return;
    }
    
    // Limpar com vermelho (em vez de verde)
    p_glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    p_glClear(0x00004000); // GL_COLOR_BUFFER_BIT
}
