//
// Created by Admin on 2023/5/28.
//
#include "jni_def.h"
#include "JNI_OnLoad.h"
#include "stb_image.h"
#include "java-Objects.h"
#include "m2g/Image.h"
#include "m2g/ImageCodec.h"


jlong NativeImageCodec_CreateImageWH(JNIEnv *, jclass, jint width, jint height) {
    if(auto* image = ImageCodec::createImage(width, height)) {
        objects::increase();
        return reinterpret_cast<jlong>(image);
    }
    return 0;
}

jlong NativeImageCodec_CreateImageFormImage(JNIEnv *, jclass, jlong handle) {
    if(auto* image = reinterpret_cast<Image*>(handle)) {
        objects::increase();
        return reinterpret_cast<jlong>(ImageCodec::createImage(image));
    }
    return 0;
}

jlong NativeImageCodec_CreateImageFromData(JNIEnv *env, jclass, jbyteArray imageData, jint imageOffset, jint imageLength) {
    objects::increase();
    jbyte* data = jniGetByteArrayElements(env, imageData, nullptr);
    Image* image = ImageCodec::loadImage(reinterpret_cast<unsigned char *> (data), imageOffset, imageLength);
    jniReleaseByteArrayElements(env, imageData, data, JNI_ABORT);
    return reinterpret_cast<jlong>(image);
}

jlong NativeImageCodec_CreateRGBImage(JNIEnv *env, jclass, jintArray rgb, jint width, jint height, jboolean processAlpha) {
    objects::increase();
    jint* rgbData = jniGetIntArrayElements(env, rgb, nullptr);
    Image* image = ImageCodec::createRGBImage(rgbData, width, height, processAlpha);
    jniReleaseIntArrayElements(env, rgb, rgbData, JNI_ABORT);
    return reinterpret_cast<jlong>(image);
}


jlong NativeImageCodec_CreateImage(JNIEnv *env, jclass, jlong handle,jint  x, jint y, jint width, jint height, jint transform) {
    if(auto* image = reinterpret_cast<Image*>(handle)) {
        objects::increase();
        return reinterpret_cast<jlong>(ImageCodec::createImage(image, x, y, width, height, transform));
    }
    return 0L;
}



int register_m2g_ImageCodec(JNIEnv* env) {
    static const JNINativeMethod methods[] = {
            {"jniCreateImage"        , "(II)J"          , reinterpret_cast<void*>(NativeImageCodec_CreateImageWH)      },
            {"jniCreateImage"        , "(J)J"          , reinterpret_cast<void*>(NativeImageCodec_CreateImageFormImage)      },
            {"jniCreateImage"        , "([BII)J"          , reinterpret_cast<void*>(NativeImageCodec_CreateImageFromData)      },
            {"jniCreateRGBImage"        , "([IIIZ)J"          , reinterpret_cast<void*>(NativeImageCodec_CreateRGBImage)      },
            {"jniCreateImage"        , "(JIIIII)J"          , reinterpret_cast<void*>(NativeImageCodec_CreateImage)      },
    };

    const auto clazz = jniFindClass(env, "iml/m2g/NativeImageCodec");
    return clazz
           ? jniRegisterNatives(env, clazz, methods, std::size(methods))
           : JNI_ERR;
}