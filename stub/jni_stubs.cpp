// Minimal ORIGINAL arm64 JNI stub for experimentation.
// Does not contain Gameloft engine code.
// Build with Android NDK clang++ for aarch64; link -llog only as needed.

#include <jni.h>
#include <android/log.h>

#define LOG_TAG "NOVA3_stub"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    LOGI("JNI_OnLoad — original minimal arm64 stub");
    return JNI_VERSION_1_6;
}

static void noop(JNIEnv*, jclass) {}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_init(JNIEnv*, jclass) {
    LOGI("init()");
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_initGL(JNIEnv*, jclass) {
    LOGI("initGL()");
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_step(JNIEnv*, jclass) {
    // Frame tick — no real renderer
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_resize(JNIEnv*, jclass, jint w, jint h) {
    LOGI("resize(%d,%d)", (int)w, (int)h);
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_setPaths(
    JNIEnv* env, jclass, jstring a, jstring b, jstring c, jstring d) {
    LOGI("setPaths(...)");
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeSetIsMOGA(JNIEnv*, jclass, jint v) {
    LOGI("nativeSetIsMOGA(%d)", (int)v);
}

// Installer helpers often required to pass early Java flows — still no engine.
extern "C" JNIEXPORT jstring JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_installer_GameInstaller_getPublicKey(
    JNIEnv* env, jclass) {
    // Placeholder: replace with your own test key material if you exercise LVL locally.
    // Do not commit third-party private keys.
    return env->NewStringUTF("");
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_installer_GameInstaller_initNative(JNIEnv*, jclass) {
    LOGI("GameInstaller.initNative()");
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_installer_GameInstaller_nativeStart(JNIEnv*, jclass) {
    LOGI("GameInstaller.nativeStart()");
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_iab_InAppBilling_nativeInit(JNIEnv*, jclass, jobject) {
    LOGI("InAppBilling.nativeInit()");
}
