//
// Created by Admin on 2023/5/28.
//

#include "JNI_OnLoad.h"
#include "jni_def.h"


#define REGISTER_NATIVES(class_name)              \
extern int register_m2g_##class_name(JNIEnv*);    \
if (auto rc = register_m2g_##class_name(env)) {   \
    return rc;                                    \
}


#ifdef M2G_BUILD_SHARED
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
#else
JNIEXPORT jint M2G_OnLoad(JavaVM* vm, void* reserved) {
#endif
    JNIEnv* env;
    if (jniGetEnv(vm, reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    REGISTER_NATIVES(Image)
    REGISTER_NATIVES(ImageCodec)
    REGISTER_NATIVES(Graphics)
    REGISTER_NATIVES(Font)
    return JNI_VERSION_1_6;
}
