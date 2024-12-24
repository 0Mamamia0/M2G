

#include <array>

#include "jni_def.h"
#include "java-Objects.h"
#include "Image.h"


using namespace m2g;


static jint nGetWidth(JNIEnv *, jclass, jlong handle) {
    if (auto *image = reinterpret_cast<Image*>(handle)) {
        return image->getWidth();
    }
    return 0;
}

static jint nGetHeight(JNIEnv *, jclass, jlong handle) {
    if (auto *image = reinterpret_cast<Image*>(handle)) {
        return image->getHeight();
    }
    return 0;
}

static jboolean nIsMutable(JNIEnv *, jclass, jlong handle) {
    if (auto *image = reinterpret_cast<Image *>(handle)) {
        return image->isMutable();
    }
    return false;
}

static void nGetRGB(JNIEnv *env, jclass, jlong handle, jintArray data, jint dataLength, jint offset, jint scanLength, jint x, jint y, jint width, jint height) {
    if (auto *image = reinterpret_cast<Image *>(handle)) {
        jboolean isCopy;
        jint *argb = jniGetIntArrayElements(env, data, &isCopy);
        image->getRGB(argb, dataLength, offset, scanLength, x, y, width, height);
        jniReleaseIntArrayElements(env, data, argb, 0);
    }
}

static jlong nGetPixelsAdders(JNIEnv *, jclass, jlong handle) {
     if(auto* image = reinterpret_cast<Image*>(handle)) {
         return reinterpret_cast<jlong>(image->getPixels());
     }
    return 0;
}



static void nRelease(JNIEnv *, jclass, jlong handle) {
    auto *image = reinterpret_cast<Image *>(handle);
    delete image;
}


static jint  nGetFormat(JNIEnv *, jclass, jlong handle) {
    if (auto *image = reinterpret_cast<Image *>(handle)) {
        return image->getFormat();
    }
    return -1;
};


extern "C" int register_m2g_Image(JNIEnv *env) {

#define BIND_JNI(fun, sig) { const_cast<char*>(#fun), const_cast<char*>(sig), reinterpret_cast<void*>(fun)}
    static const JNINativeMethod methods[] = {
            BIND_JNI(nGetWidth, "(J)I"),
            BIND_JNI(nGetHeight, "(J)I"),
            BIND_JNI(nIsMutable, "(J)Z"),
            BIND_JNI(nGetRGB, "(J[IIIIIIII)V"),
            BIND_JNI(nRelease, "(J)V"),
            BIND_JNI(nGetPixelsAdders, "(J)J"),
            BIND_JNI(nGetFormat, "(J)I"),
    };

    const auto clazz = jniFindClass(env, "iml/m2g/NativeImage");
    return clazz
           ? jniRegisterNatives(env, clazz, methods, std::size(methods))
           : JNI_ERR;
}