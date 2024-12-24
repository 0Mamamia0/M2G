#include "java-Typeface.h"
#include <memory>
#include "jni_def.h"
#include "m2g.h"


using namespace m2g;
static jlong nCreate(JNIEnv *env, jclass clazz, jstring path) {
    const char* str = jniGetStringUTFChars(env, path, nullptr);
    Typeface* typeface = Typeface::makeFormFile(str);
    jniReleaseStringUTFChars(env, path, str);
    return reinterpret_cast<jlong>(typeface);
}


static void nRelease(JNIEnv *env, jclass clazz, jlong handle) {
    delete reinterpret_cast<Typeface*>(handle);
}


extern "C" int register_m2g_Typeface(JNIEnv *env) {
#define BIND_JNI(fun, sig) { const_cast<char*>(#fun), const_cast<char*>(sig), reinterpret_cast<void*>(fun)}
    static const JNINativeMethod methods[] = {
            BIND_JNI(nCreate, "(Ljava/lang/String;)J"),
            BIND_JNI(nRelease, "(J)V")
    };

    const auto clazz = jniFindClass(env, "iml/m2g/M2GTypeface");
    jint res = jniRegisterNatives(env, clazz, methods, std::size(methods));
    return clazz ? res: JNI_ERR;
}