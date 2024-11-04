

#ifndef JNI_ONLOAD_H
#define JNI_ONLOAD_H

#include "jni_def.h"


#ifdef __cplusplus
extern "C" {
#endif
    JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved);
#ifdef __cplusplus
}
#endif


#endif //JNI_ONLOAD_H
