#include <jni.h>
#include <android/log.h>
#include <cstdio>
#include <cstring>

#define LOG_TAG "NOVA3_engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Incluir o motor
#include "engine/glf_app.h"
extern void test_all_files();

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    __android_log_print(ANDROID_LOG_ERROR, "NOVA3_engine", "=== JNI_OnLoad CHAMADO (ERROR) ===");
    LOGI("JNI_OnLoad — clean stub");
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_init(JNIEnv*, jclass) {
    __android_log_print(ANDROID_LOG_ERROR, "NOVA3_engine", "=== INIT JNI CHAMADO (ERROR) ===");
    LOGI("init() -> glf::App");
    static glf::App s_app;
    glf::CreationSettings settings;
    settings.width = 1280;
    settings.height = 720;
    settings.flags = 0;
    s_app.Init(settings);
    s_app.MyInit();
    s_app.Run();
    glf::App::SetInstance(&s_app);
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_step(JNIEnv*, jclass) {
    glf::App* app = glf::App::GetInstance();
    if (app) app->Update();
}

// Stubs para todas as outras funções JNI
#define STUB(name) \
extern "C" JNIEXPORT void JNICALL \
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_##name(JNIEnv*, jclass) { LOGI(#name); }

STUB(initGL)
STUB(resize)
STUB(setPaths)
STUB(nativeSetIsMOGA)
STUB(setResourcePath)

// GameInstaller
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_installer_GameInstaller_initNative(JNIEnv*, jobject) {
    LOGI("GameInstaller.initNative");
}
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_installer_GameInstaller_nativeStart(JNIEnv*, jobject) {
    LOGI("GameInstaller.nativeStart");
}

// Push Notification C2DM
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_PushNotification_C2DMAndroidUtils_nativeInit(JNIEnv*, jclass, jobject) {
    __android_log_print(ANDROID_LOG_ERROR, "NOVA3_engine", "C2DMAndroidUtils.nativeInit");
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeIsSlideChanged(JNIEnv*, jclass, jboolean) {
    return JNI_FALSE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_InitViewSettings(JNIEnv*, jclass) {
    __android_log_print(ANDROID_LOG_INFO, "NOVA3_engine", "InitViewSettings stub");
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_SetDepthValue(JNIEnv*, jclass, jint) {
    __android_log_print(ANDROID_LOG_INFO, "NOVA3_engine", "SetDepthValue stub");
}

extern "C" JNIEXPORT jint JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_getNumExtraContext(JNIEnv*, jclass) {
    __android_log_print(ANDROID_LOG_INFO, "NOVA3_engine", "getNumExtraContext stub -> 0");
    return 0;
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_stateChanged(JNIEnv*, jclass, jboolean) {
    __android_log_print(ANDROID_LOG_INFO, "NOVA3_engine", "stateChanged stub");
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativePowerStatus(JNIEnv*, jclass, jboolean) {
    __android_log_print(ANDROID_LOG_INFO, "NOVA3_engine", "nativePowerStatus stub");
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_GamePause(JNIEnv*, jclass) {
    __android_log_print(ANDROID_LOG_INFO, "NOVA3_engine", "GamePause stub");
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_GameResume(JNIEnv*, jclass) {
    __android_log_print(ANDROID_LOG_INFO, "NOVA3_engine", "GameResume stub");
}

