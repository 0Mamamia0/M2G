//
// Created by Admin on 2023/12/1.
//

#include "jni_def.h"
#include <array>
#include <cstring>
#include <memory>

#include "JNI_OnLoad.h"
#include "m2g/Font.h"


namespace {
    std::shared_ptr<Typeface> DEFAULT_TYPEFACE = Typeface::makeFormFile("/simkai.ttf");
}



jlong NativeFont_Create(JNIEnv *, jclass, jint face, jint style, jint size) {
    return reinterpret_cast<jlong>(new Font(DEFAULT_TYPEFACE.get(), face, style, size));
}

jint NativeFont_GetHeight(JNIEnv *, jclass, jlong handle) {
    if(auto* font = reinterpret_cast<Font*>(handle)) {
        return font->getHeight();
    }
    return 0;
}

jint NativeFont_GetBaselinePosition(JNIEnv *, jclass, jlong handle) {
    if(auto* font = reinterpret_cast<Font*>(handle)) {
        return font->getBaseLinePosition();
    }
    return 0;
}

jint NativeFont_StringWidth(JNIEnv* env, jclass, jlong handle, jstring str) {
    if (auto* font = reinterpret_cast<Font *>(handle)) {
        const char* text = jniGetStringUTFChars(env, str, nullptr);
        int width = font->charsWidth(text, strlen(text));
        jniReleaseStringUTFChars(env, str, text);
        return width;
    }
    return 0;
}


//jint NativeFont_CharsWidth(JNIEnv *, jclass, jlong font, jcharArray chars, jint offset, jint count);
//jint NativeFont_SubstringWidth(JNIEnv *, jclass, jlong font, jstring str, jint offset, jint count);
void NativeFont_Release(JNIEnv *, jclass, jlong handle) {
    if(auto* font = reinterpret_cast<Font *>(handle)) {
        delete font;
    }
}



extern int register_m2g_Font(JNIEnv *env) {
    static const JNINativeMethod methods[] = {
            {"jniCreateFont", "(III)J", reinterpret_cast<void *>(NativeFont_Create)},
            {"jniGetHeight", "(J)I", reinterpret_cast<void *>(NativeFont_GetHeight)},
            {"jniGetBaselinePosition", "(J)I", reinterpret_cast<void *>(NativeFont_GetBaselinePosition)},
//            {"jniCharWidth",         "(J)V", reinterpret_cast<void *>()}
//            {"jniCharsWidth",         "(J)V", reinterpret_cast<void *>()}
            {"jniStringWidth",         "(JLjava/lang/String;)I", reinterpret_cast<void *>(NativeFont_StringWidth)},
//            {"jniSubstringWidth",         "(J)V", reinterpret_cast<void *>()}
            {"jniRelease",         "(J)V", reinterpret_cast<void *>(NativeFont_Release)}
    };

    const auto clazz = jniFindClass(env, "iml/m2g/NativeFont");
    return clazz ? jniRegisterNatives(env, clazz, methods, std::size(methods))
           : JNI_ERR;
}