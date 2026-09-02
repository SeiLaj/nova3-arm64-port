#include <jni.h>
#include <android/log.h>
#include <cstdio>
#include <cstring>

#define LOG_TAG "NOVA3_stub"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" bool renderer_init(const char* path);
extern "C" void renderer_draw();

static int g_gl_ready = 0;
static int g_init_attempted = 0;

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM*, void*) {
    LOGI("=== JNI_OnLoad EXECUTADO ===");
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL
Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_step(JNIEnv*, jclass) {
    // Se ainda não inicializou, tenta no step
    if (!g_init_attempted) {
        g_init_attempted = 1;
        LOGI("step: primeira chamada — tentando inicializar renderer");
        const char* path = "/data/data/com.gameloft.android.ANMP.GloftN3HM/files/B1_ground_rock.etc";
        if (renderer_init(path)) {
            g_gl_ready = 1;
            LOGI("step: renderer_init SUCESSO!");
        } else {
            LOGI("step: renderer_init FALHOU!");
        }
    }
    
    if (g_gl_ready) {
        renderer_draw();
    } else {
        // Não loga a cada frame para não poluir
    }
}

// Stubs mínimos
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_init(JNIEnv*, jclass) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_initGL(JNIEnv*, jclass) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_resize(JNIEnv*, jclass, jint, jint) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_setPaths(JNIEnv*, jclass, jstring, jstring, jstring, jstring) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeSetIsMOGA(JNIEnv*, jclass, jint) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_setResourcePath(JNIEnv*, jclass, jstring) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativePowerStatus(JNIEnv*, jclass, jboolean) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeSetXperiaPlay(JNIEnv*, jclass, jboolean) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeSetSO01D(JNIEnv*, jclass, jboolean) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_destroy(JNIEnv*, jclass) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_InitViewSettings(JNIEnv*, jclass) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_GamePause(JNIEnv*, jclass) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_GameResume(JNIEnv*, jclass) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_stateChanged(JNIEnv*, jclass, jint) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_SetDepthValue(JNIEnv*, jclass, jfloat) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_SetUsedGPU(JNIEnv*, jclass, jint) {}
extern "C" JNIEXPORT jint JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_getNumExtraContext(JNIEnv*, jclass) { return 0; }
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_setNumExtraContext(JNIEnv*, jclass, jint) {}
extern "C" JNIEXPORT jboolean JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeIsMainMenuOrIGM(JNIEnv*, jclass) { return JNI_FALSE; }
extern "C" JNIEXPORT jboolean JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeIsSlideChanged(JNIEnv*, jclass, jboolean) { return JNI_FALSE; }

// GameInstaller
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_installer_GameInstaller_initNative(JNIEnv*, jobject) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_installer_GameInstaller_nativeStart(JNIEnv*, jobject) {}
extern "C" JNIEXPORT jstring JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_installer_GameInstaller_getPublicKey(JNIEnv* env, jclass) {
    return env->NewStringUTF("MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy0REY8A0AQAB");
}

// GDRM
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GDRMPolicy_initNativeAP(JNIEnv*, jclass) {}
extern "C" JNIEXPORT jstring JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GDRMPolicy_getConstString(JNIEnv* env, jclass, jint) {
    return env->NewStringUTF("");
}

// IAB / Push
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_iab_InAppBilling_nativeInit(JNIEnv*, jclass, jobject) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_PushNotification_C2DMAndroidUtils_nativeInit(JNIEnv*, jclass, jobject) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_PushNotification_C2DMAndroidUtils_nativeInit__(JNIEnv*, jclass) {}

// Input
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_OnKeyDown(JNIEnv*, jclass, jint) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_OnKeyUp(JNIEnv*, jclass, jint) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_OnKeyboardFinish(JNIEnv*, jclass) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_accelerometerEvent(JNIEnv*, jclass, jfloat, jfloat, jfloat) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_gyroscopeEvent(JNIEnv*, jclass, jfloat, jfloat, jfloat) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeMogaVersion(JNIEnv*, jclass, jint) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeSetPowerALeftJoystick(JNIEnv*, jclass, jfloat, jfloat, jfloat) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_nativeSetPowerARightJoystick(JNIEnv*, jclass, jfloat, jfloat, jfloat) {}
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_orientationChanged(JNIEnv*, jclass, jint) {}
extern "C" JNIEXPORT jboolean JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_processTouchpadAsPointer(JNIEnv*, jclass, jint, jobject, jboolean) { return JNI_FALSE; }
extern "C" JNIEXPORT void JNICALL Java_com_gameloft_android_ANMP_GloftN3HM_GL2JNILib_touchEvent(JNIEnv*, jclass, jint, jint, jint, jint) {}
