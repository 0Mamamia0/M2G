#pragma once
#include "jni_def.h"


#ifdef __cplusplus
extern "C" {
#endif
    int register_m2g_Image(JNIEnv *env);
    int register_m2g_ImageCodec(JNIEnv *env);
    int register_m2g_Graphics(JNIEnv *env);
    int register_m2g_Typeface(JNIEnv *env);
    int register_m2g_Font(JNIEnv *env);
    int register_m2g_Objects(JNIEnv *env);

    jint M2G_OnLoad(JavaVM *vm, void *reserved);
#ifdef __cplusplus
}
#endif



