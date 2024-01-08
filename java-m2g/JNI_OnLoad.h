//
// Created by Admin on 2023/5/28.
//

#ifndef LEARNSFML_JNI_ONLOAD_H
#define LEARNSFML_JNI_ONLOAD_H

#include "jni_def.h"


#ifdef __cplusplus
extern "C" {
#endif



extern int register_m2g_Image(JNIEnv *env);
extern int register_m2g_ImageCodec(JNIEnv *env);
extern int register_m2g_Graphics(JNIEnv *env);
extern int register_m2g_Font(JNIEnv *env);

#ifdef M2G_BUILD_SHARED
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved);
#else
JNIEXPORT jint M2G_OnLoad(JavaVM *vm, void *reserved);
#endif


#ifdef __cplusplus
}
#endif

#endif //LEARNSFML_JNI_ONLOAD_H
