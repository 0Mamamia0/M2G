//
// Created by Admin on 2023/12/3.
//

#ifndef JNI_DEF_H__
#define JNI_DEF_H__


#include "jni.h"


#ifdef USE_JAMVM
#define JNI_C_STYLE
#endif


#ifdef __cplusplus
extern "C"{
#endif

    inline jint jniGetEnv(JavaVM *vm, void **penv, jint version)
    {
#ifdef JNI_C_STYLE
        return (*vm)->GetEnv(vm, penv, version);
#else
        return vm->GetEnv((void **)penv, version);
#endif
    };

    inline jclass jniFindClass(JNIEnv *env, const char *name) {
#ifdef JNI_C_STYLE
        return (*env)->FindClass(env, name);
#else
        return env->FindClass(name);
#endif
    }

    inline jint jniRegisterNatives(JNIEnv *env, jclass clazz, const JNINativeMethod *methods, jint nMethods) {
#ifdef JNI_C_STYLE
        return (*env)->RegisterNatives(env, clazz, methods, nMethods);
#else
        return env->RegisterNatives(clazz, methods, nMethods);
#endif
    }

    inline const char*  jniGetStringUTFChars(JNIEnv *env, jstring str, jboolean *isCopy) {
#ifdef JNI_C_STYLE
        return (*env)->GetStringUTFChars(env, str, isCopy);
#else
        return env->GetStringUTFChars(str, isCopy);
#endif
    }

    inline void jniReleaseStringUTFChars(JNIEnv *env, jstring str, const char *chars) {
#ifdef JNI_C_STYLE
        (*env)->ReleaseStringUTFChars(env, str, chars);
#else
        env->ReleaseStringUTFChars(str, chars);
#endif

    }

    inline jbyte* jniGetByteArrayElements(JNIEnv *env, jbyteArray array, jboolean *isCopy) {
#ifdef JNI_C_STYLE
        return (*env)->GetByteArrayElements(env, array, isCopy);
#else
        return env->GetByteArrayElements(array, isCopy);
#endif
    }


inline void jniReleaseByteArrayElements(JNIEnv *env, jbyteArray array, jbyte* elems, jint mode) {
#ifdef JNI_C_STYLE
    (*env)->ReleaseByteArrayElements(env, array, elems, mode);
#else
    env->ReleaseByteArrayElements(array, elems, mode);
#endif
}


    inline jint* jniGetIntArrayElements(JNIEnv *env,jintArray array, jboolean* isCopy ) {
#ifdef JNI_C_STYLE
        return (*env)->GetIntArrayElements(env, array, isCopy);
#else
        return env->GetIntArrayElements(array, isCopy);
#endif
    }

    inline void jniReleaseIntArrayElements(JNIEnv *env, jintArray array, jint *elems, jint mode) {
#ifdef JNI_C_STYLE
        return (*env)->ReleaseIntArrayElements(env, array, elems, mode);
#else
        return env->ReleaseIntArrayElements(array, elems, mode);
#endif
    }






#ifdef __cplusplus
};
#endif


#endif //JNI_DEF_H__
