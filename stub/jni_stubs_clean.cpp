#include <jni.h>
#include <android/log.h>
#include <cstdio>
#include <cstring>

#define LOG_TAG "NOVA3_engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Incluir o motor
#include "engine/glf_app.h"

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    LOGI("JNI_OnLoad — clean stub");
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_init(JNIEnv*, jclass) {
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
    LOGI("C2DMAndroidUtils.nativeInit");
}
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_PushNotification_C2DMAndroidUtils_nativeInit__(JNIEnv* env, jclass clazz) {
    LOGI("C2DMAndroidUtils.nativeInit (no context)");
}

// GL2JNILib extras
extern "C" JNIEXPORT jboolean JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeIsSlideChanged(JNIEnv*, jclass, jboolean) {
    LOGI("nativeIsSlideChanged");
    return JNI_FALSE;
}

// GL2JNILib - funções adicionais
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeSetPowerALeftJoystick(JNIEnv*, jclass, jfloat, jfloat, jfloat) {
    LOGI("nativeSetPowerALeftJoystick");
}
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeSetPowerARightJoystick(JNIEnv*, jclass, jfloat, jfloat, jfloat) {
    LOGI("nativeSetPowerARightJoystick");
}
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeMogaVersion(JNIEnv*, jclass, jint) {
    LOGI("nativeMogaVersion");
}
extern "C" JNIEXPORT jboolean JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeIsMainMenuOrIGM(JNIEnv*, jclass) {
    LOGI("nativeIsMainMenuOrIGM");
    return JNI_FALSE;
}
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_GamePause(JNIEnv*, jclass) {
    LOGI("GamePause");
}
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_GameResume(JNIEnv*, jclass) {
    LOGI("GameResume");
}
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_InitViewSettings(JNIEnv*, jclass) {
    LOGI("InitViewSettings");
}
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_SetDepthValue(JNIEnv*, jclass, jfloat) {
    LOGI("SetDepthValue");
}
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_SetUsedGPU(JNIEnv*, jclass, jint) {
    LOGI("SetUsedGPU");
}
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_touchEvent(JNIEnv*, jclass, jint, jint, jint, jint) {
    // LOGI("touchEvent"); // muito ruído
}
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_OnKeyDown(JNIEnv*, jclass, jint) {
    // LOGI("OnKeyDown");
}
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_OnKeyUp(JNIEnv*, jclass, jint) {
    // LOGI("OnKeyUp");
}

// GL2JNILib - getNumExtraContext
extern "C" JNIEXPORT jint JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_getNumExtraContext(JNIEnv*, jclass) {
    LOGI("getNumExtraContext");
    return 0; // Sem contextos extras
}

// GL2JNILib - stateChanged
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_stateChanged(JNIEnv*, jclass, jboolean) {
    LOGI("stateChanged");
}

// GL2JNILib - nativePowerStatus
extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativePowerStatus(JNIEnv*, jclass, jboolean) {
    LOGI("nativePowerStatus");
}
