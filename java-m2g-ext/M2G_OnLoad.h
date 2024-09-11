//
// Created by Admin on 2023/5/28.
//

#ifndef JNI_ONLOAD_H
#define JNI_ONLOAD_H

#include "jni_def.h"


#ifdef __cplusplus
extern "C" {
#endif
    int register_m2g_Image(JNIEnv *env);
    int register_m2g_ImageCodec(JNIEnv *env);
    int register_m2g_Graphics(JNIEnv *env);
    int register_m2g_Font(JNIEnv *env);
    int register_m2g_Objects(JNIEnv *env);
    jint M2G_OnLoad(JavaVM *vm, void *reserved);
#ifdef __cplusplus
}
#endif


#endif //JNI_ONLOAD_H
