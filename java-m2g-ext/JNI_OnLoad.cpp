

#include "jni_def.h"
#include "M2G_OnLoad.h"



JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    return M2G_OnLoad(vm, reserved);
}

