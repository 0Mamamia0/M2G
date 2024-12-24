
#include <array>
#include "jni_def.h"
#include "stb_image.h"
#include "java-Objects.h"
#include "ImageCodec.h"


using namespace m2g;

jlong nCreateImage(JNIEnv *, jclass, jint width, jint height) {
    if(auto* image = ImageCodec::createImage(width, height)) {
        return reinterpret_cast<jlong>(image);
    }
    return 0;
}

jlong nCreateImage(JNIEnv *, jclass, jlong handle) {
    if(auto* image = reinterpret_cast<Image*>(handle)) {
        return reinterpret_cast<jlong>(ImageCodec::createImage(image));
    }
    return 0;
}

jlong nCreateImage(JNIEnv *env, jclass, jbyteArray imageData, jint imageOffset, jint imageLength) {
    jbyte* data = jniGetByteArrayElements(env, imageData, nullptr);
    Image* image = ImageCodec::loadImage(reinterpret_cast<unsigned char *> (data), imageOffset, imageLength);
    jniReleaseByteArrayElements(env, imageData, data, JNI_ABORT);
    return reinterpret_cast<jlong>(image);
}

jlong nCreateImage(JNIEnv *env, jclass, jlong handle,jint  x, jint y, jint width, jint height, jint transform) {
    if(auto* image = reinterpret_cast<Image*>(handle)) {
        return reinterpret_cast<jlong>(ImageCodec::createImage(image, x, y, width, height, transform));
    }
    return 0L;
}


jlong nCreateRGBImage(JNIEnv *env, jclass, jintArray rgb, jint width, jint height, jboolean processAlpha) {
    jint* rgbData = jniGetIntArrayElements(env, rgb, nullptr);
    Image* image = ImageCodec::createRGBImage(rgbData, width, height, processAlpha);
    jniReleaseIntArrayElements(env, rgb, rgbData, JNI_ABORT);
    return reinterpret_cast<jlong>(image);
}





extern "C" int register_m2g_ImageCodec(JNIEnv* env) {

#define BIND_JNI(fun, sig, ptr) { const_cast<char*>(#fun), const_cast<char*>(sig), reinterpret_cast<void*>(ptr)}
    static const JNINativeMethod methods[] = {
            BIND_JNI(nCreateImage, "(II)J", static_cast<jlong(*)(JNIEnv*,jclass,jint, jint)>(nCreateImage)),
            BIND_JNI(nCreateImage, "(J)J", static_cast<jlong(*)(JNIEnv*,jclass,jlong)>(nCreateImage)),
            BIND_JNI(nCreateImage, "([BII)J", static_cast<jlong(*)(JNIEnv*,jclass,jbyteArray, jint, jint)>(nCreateImage)),
            BIND_JNI(nCreateImage, "(JIIIII)J", static_cast<jlong(*)(JNIEnv*,jclass,jlong, jint, jint, jint, jint, jint)>(nCreateImage)),
            BIND_JNI(nCreateRGBImage, "([IIIZ)J",nCreateRGBImage)
    };


    const auto clazz = jniFindClass(env, "iml/m2g/NativeImageCodec");
    return clazz
           ? jniRegisterNatives(env, clazz, methods, std::size(methods))
           : JNI_ERR;
}