
#include "engine/test_all_files.cpp"
#include "glf_app.h"
#include <android/log.h>
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
