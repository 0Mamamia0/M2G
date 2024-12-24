
#include "jni_def.h"
#include <array>
#include <cstring>
#include <memory>

#include "JNI_OnLoad.h"
#include "Font.h"
#include "java-Objects.h"


using namespace m2g;





static jlong nCreate(JNIEnv *, jclass, jlong tf, jint face, jint style, jint size) {
    if (auto* typeface = reinterpret_cast<Typeface*>(tf)) {
        return reinterpret_cast<jlong>(new Font(typeface, face, style, size));
    }
    return 0L;
}

static jint nGetHeight(JNIEnv *, jclass, jlong handle) {
    if (auto *font = reinterpret_cast<Font *>(handle)) {
        return font->getHeight();
    }
    return 0;
}

static jint nGetBaselinePosition(JNIEnv *, jclass, jlong handle) {
    if (auto *font = reinterpret_cast<Font *>(handle)) {
        return font->getBaseLinePosition();
    }
    return 0;
}

static jint nStringWidth(JNIEnv *env, jclass, jlong handle, jstring str) {
    if (auto *font = reinterpret_cast<Font *>(handle)) {
        const char *text = jniGetStringUTFChars(env, str, nullptr);
        jsize len = jniGetStringUTFLength(env, str);
        int width = font->charsWidth(text, len);
        jniReleaseStringUTFChars(env, str, text);
        return width;
    }
    return 0;
}


static jint nCharWidth(JNIEnv *, jclass, jlong handle, jchar c) {
    return 12;
}

static jint nCharsWidth(JNIEnv *env, jclass, jlong font, jcharArray chars, jint offset, jint count) {

    return 100;
};




static void nRelease(JNIEnv *, jclass, jlong handle) {
    delete reinterpret_cast<Font *>(handle);
}


extern "C" int register_m2g_Font(JNIEnv *env) {
#define BIND_JNI(fun, sig) { const_cast<char*>(#fun), const_cast<char*>(sig), reinterpret_cast<void*>(fun)}
    static const JNINativeMethod methods[] = {
            BIND_JNI(nCreate, "(JIII)J"),
            BIND_JNI(nGetHeight, "(J)I"),
            BIND_JNI(nGetBaselinePosition, "(J)I"),
            BIND_JNI(nCharWidth, "(JC)I"),
            BIND_JNI(nCharsWidth, "(J[CII)I"),
            BIND_JNI(nStringWidth, "(JLjava/lang/String;)I"),
            BIND_JNI(nRelease, "(J)V"),
    };

    const auto clazz = jniFindClass(env, "iml/m2g/NativeFont");
    return clazz ? jniRegisterNatives(env, clazz, methods, std::size(methods)) : JNI_ERR;
}