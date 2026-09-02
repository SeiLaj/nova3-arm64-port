#include "glf_app.h"
#include <android/log.h>
#include "test_all_files.cpp"

// Declarações extern do renderizador
extern "C" bool init_gles();
extern "C" bool init_renderer();
extern "C" void render_textured_quad();

#define LOG_TAG "NOVA3_engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

glf::App* glf::App::s_instance = nullptr;

glf::App::App() : m_initialized(false), m_running(false) {
    LOGI("App::App()");
}
glf::App::~App() {
    LOGI("App::~App()");
}
bool glf::App::Init(glf::CreationSettings& s) {
    LOGI("App::Init(%dx%d)", s.width, s.height);
    m_initialized = true;
    LOGI("=== CHAMANDO init_renderer() ===");
    init_renderer();
    LOGI("=== CHAMANDO render_textured_quad() ===");
    render_textured_quad();
    LOGI("=== INIT FINALIZADO ===");
    return true;
}
bool glf::App::MyInit() {
    LOGI("App::MyInit()");
    test_all_files();
    return true;
}
void glf::App::Run() {
    LOGI("App::Run()");
    m_running = true;
}
void glf::App::Update() {
    // Forçar renderização mesmo se m_running for false
    render_textured_quad();
    if (!m_running) return;
}
void glf::App::Deinit() {
    LOGI("App::Deinit()");
    m_running = false;
}
glf::App* glf::App::GetInstance() {
    return s_instance;
}
void glf::App::SetInstance(glf::App* a) {
    s_instance = a;
}
