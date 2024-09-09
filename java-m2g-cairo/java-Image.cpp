//
// Created by Admin on 2023/5/27.
//

#include <array>

#include "jni_def.h"
#include "JNI_OnLoad.h"
#include "java-Objects.h"
#include "Image.h"



using namespace m2g;



static jint NativeImage_GetWidth (JNIEnv *, jclass, jlong handle) {
    if(auto* image = reinterpret_cast<Image*>(handle)) {
        return image->getWidth();
    }
    return 0;
}

static jint NativeImage_GetHeight (JNIEnv *, jclass, jlong handle) {
    if(auto* image = reinterpret_cast<Image*>(handle)) {
        return image->getHeight();
    }
    return 0;
}

static jboolean NativeImage_IsMutable(JNIEnv *, jclass, jlong handle) {
    if(auto* image = reinterpret_cast<Image*>(handle)) {
        return image->isMutable();
    }
    return false;
}

static void NativeImage_GetRGB(JNIEnv *env, jclass, jlong handle, jintArray data, jint dataLength, jint offset, jint scanLength, jint x, jint y, jint width, jint height) {
    if(auto* image = reinterpret_cast<Image*>(handle)) {
        jboolean isCopy;
        jint* argb = jniGetIntArrayElements(env, data, &isCopy);
        image->getRGB( argb, dataLength, offset, scanLength, x, y, width, height);
        jniReleaseIntArrayElements(env, data, argb, 0);
    }
}

static jlong NativeImage_GetPixelsAdders(JNIEnv *, jclass, jlong handle) {
    // if(auto* image = reinterpret_cast<Image*>(handle)) {
    //     return image->getPixelBufferRef().addr<jlong>();
    // }
    return 0;
}

static jint NativeImage_GetChannels(JNIEnv *, jclass, jlong handle) {
    // if(auto* image = reinterpret_cast<Image*>(handle)) {
    //     return image->getPixelBufferRef().getBytePerPixel();
    // }
    return 0;
}

static void NativeImage_Release(JNIEnv *, jclass, jlong handle) {
    auto* image = reinterpret_cast<Image*>(handle);
    delete image;
    objects::decrease();
}





extern "C" int register_m2g_Image(JNIEnv* env) {
    static const JNINativeMethod methods[] = {
            {"jniGetWidth"      , "(J)I"           , reinterpret_cast<void*>(NativeImage_GetWidth)      },
            {"jniGetHeight"     , "(J)I"           , reinterpret_cast<void*>(NativeImage_GetHeight)      },
            {"jniIsMutable"     , "(J)Z"           , reinterpret_cast<void*>(NativeImage_IsMutable)      },
            {"jniGetRGB"        , "(J[IIIIIIII)V"  , reinterpret_cast<void*>(NativeImage_GetRGB)      },
            {"jniRelease"       , "(J)V"           , reinterpret_cast<void*>(NativeImage_Release)      },
            {"jniGetPixelsAdders"       , "(J)J"   , reinterpret_cast<void*>(NativeImage_GetPixelsAdders)      },
            {"jniGetChannels"       , "(J)I"       , reinterpret_cast<void*>(NativeImage_GetChannels)      },
    };

    const auto clazz = jniFindClass(env, "iml/m2g/NativeImage");
    return clazz
           ? jniRegisterNatives(env, clazz, methods, std::size(methods))
           : JNI_ERR;
}