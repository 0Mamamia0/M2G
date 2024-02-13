//
// Created by Admin on 2023/12/4.
//

#include <atomic>
#include <array>
#include "jni_def.h"


namespace objects {
    std::atomic<int> num = 0;

    void increase() {
        ++num;
    }

    void decrease() {
        --num;
    }

    int count() {
        return num.load();
    }
}




static jint NativeObjects_activeCount(JNIEnv *env, jclass) {
    return objects::count();
}



extern "C" int register_m2g_Objects(JNIEnv* env) {
    static const JNINativeMethod methods[] = {
            {"activeCount"       , "()I"       , reinterpret_cast<void*>(NativeObjects_activeCount)      },
    };
    const auto clazz = jniFindClass(env, "iml/m2g/NativeObjects");
    return clazz
           ? jniRegisterNatives(env, clazz, methods, std::size(methods))
           : JNI_ERR;
}


