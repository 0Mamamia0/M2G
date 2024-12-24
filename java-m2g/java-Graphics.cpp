﻿
#include <iostream>
#include "jni_def.h"
#include "Image.h"
#include "Graphics.h"
#include "java-Objects.h"


using namespace m2g;


static jlong nCreateFormImage(JNIEnv *, jclass, jlong handle) {
    if(auto* image = reinterpret_cast<Image*>(handle)) {
        if(image->isMutable()) {
            return reinterpret_cast<jlong>(new Graphics(image));
        }
    }
    return 0;
}




static void nRelease(JNIEnv *, jclass, jlong handle) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    delete graphics;
}


static void nDrawLine(JNIEnv *, jclass, jlong handle, jint x0, jint y0, jint x1, jint y1) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->drawLine(x0, y0, x1, y1);
    }
}

static void nDrawRect(JNIEnv *, jclass, jlong handle, jint x, jint y, jint width, jint height) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->drawRect(x, y, width, height);
    }
}

static void nFillRect(JNIEnv *, jclass, jlong handle, jint x, jint y, jint width, jint height) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->fillRect(x, y, width, height);
    }
}

static void nDrawRoundRect(JNIEnv *, jclass, jlong handle, jint x, jint y, jint width, jint height, jint arcWidth, jint arcHeight) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->drawRoundRect(x, y, width, height, arcWidth, arcHeight);
    }
}

static void nFillRoundRect(JNIEnv *, jclass, jlong handle, jint x, jint y, jint width, jint height, jint arcWidth, jint arcHeight) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->fillRoundRect(x, y, width, height, arcWidth, arcHeight);
    }
}

static void nDrawArc(JNIEnv *, jclass, jlong handle, jint x, jint y, jint width, jint height, jint arcWidth, jint arcHeight) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->drawArc(x, y, width, height, arcWidth, arcHeight);
    }
}

static void nFillArc(JNIEnv *, jclass, jlong handle, jint x, jint y, jint width, jint height, jint arcWidth, jint arcHeight) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->fillArc(x, y, width, height, arcWidth, arcHeight);
    }
}

static void nDrawCircle(JNIEnv *, jclass, jlong handle, jint centerX, jint centerY, jint r) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->drawCircle( centerX, centerY, r);
    }
}

static void nFillCircle(JNIEnv *, jclass, jlong handle, jint centerX, jint centerY, jint r) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->fillCircle(centerX, centerY, r);
    }
}


static void nDrawImageWH(JNIEnv *, jclass, jlong handle, jlong imageHandle, jint x, jint y, jint width, jint height, jint anchor) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    auto* image = reinterpret_cast<Image*>(imageHandle);
    if(graphics != nullptr && image != nullptr) {
        graphics->drawImage(image, x, y, width, height, anchor);
    }
}



static void nDrawImage(JNIEnv *, jclass, jlong handle, jlong imageHandle, jint x, jint y, jint anchor) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    auto* image = reinterpret_cast<Image*>(imageHandle);
    if(graphics != nullptr && image != nullptr) {
        graphics->drawImage(image, x, y, anchor);
    }
}


static void nSetColor(JNIEnv *, jclass, jlong handle, jint color) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->setColor(color);
    }
}


static void nTranslate(JNIEnv *, jclass, jlong handle, jint x, jint y) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->translate(x, y);
    }
}

static void nSetClip(JNIEnv *, jclass, jlong handle, jint x, jint y, jint width, jint height) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->setClip(x, y, width, height);
    }
}

static void nClipRect(JNIEnv *, jclass, jlong handle, jint x, jint y, jint width, jint height) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->clipRect(x, y, width, height);
    }
}

static jint nGetTranslateX(JNIEnv *, jclass, jlong handle) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        return graphics->getTranslateX();
    }
    return 0;
}

static jint nGetTranslateY(JNIEnv *, jclass, jlong handle) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        return graphics->getTranslateY();
    }
    return 0;
}


static jint nGetClipX(JNIEnv *, jclass, jlong handle) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        return graphics->getClipX();
    }
    return 0;
}

static jint nGetClipY(JNIEnv *, jclass, jlong handle) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        return graphics->getClipY();
    }
    return 0;
}



static jint nGetClipWidth(JNIEnv *, jclass, jlong handle) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        return graphics->getClipWidth();
    }
    return 0;
}


static jint nGetClipHeight(JNIEnv *, jclass, jlong handle) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        return graphics->getClipHeight();
    }
    return 0;
}




static void nDrawString(JNIEnv* env, jclass,  jlong handle, jstring str, jint x, jint y, jint anchor, jlong  fontHandle) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    auto* font = reinterpret_cast<const Font*>(fontHandle);
    if(graphics != nullptr && font != nullptr) {
        jsize len = jniGetStringLength(env, str);
        const jchar* text = jniGetStringCritical(env, str, nullptr);
        graphics->drawString((const char16_t*)text, len, x, y, anchor, *font);
        jniReleaseStringCritical(env, str, text);
    }
}

static void nDrawRegion(JNIEnv* env, jclass,  jlong handle, jlong imageHandle,
                                    jint x, jint y, jint width, jint height,
                                    jint transform,  jint x_dst, jint y_dst,
                                    jint anchor) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    auto* image = reinterpret_cast<Image*>(imageHandle);
    if(graphics) {
        graphics->drawRegion(image, x, y, width, height, transform, x_dst, y_dst, anchor);
    }
}

static void nCopyArea(JNIEnv* env, jclass,  jlong handle, jint x_src, jint y_src, jint width, jint height,
                             jint x_dst, jint y_dst, jint anchor) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    if(graphics) {
        graphics->copyArea(x_src, y_src, width, height, x_dst, y_dst, anchor);
    }
}


static void nDrawRGB(JNIEnv* env, jclass,  jlong handle, jintArray array, jint offset, jint scanLength,
                            jint x, jint y, jint width, jint height, jboolean processAlpha) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    if(graphics) {
        auto* data = jniGetIntArrayElements(env, array, nullptr);
        int dataLength = jniGetArrayLength(env, array);
        graphics->drawRGB(data,dataLength, offset, scanLength, x, y, width, height, processAlpha);
        jniReleaseIntArrayElements(env, array, data, JNI_ABORT);
    }
}


static jint nSave(JNIEnv* env, jclass,  jlong handle) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    if(graphics) {
        return graphics->save();
    }
    return -1;
}


static jboolean nRestore(JNIEnv* env, jclass,  jlong handle) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    if(graphics) {
        return graphics->restore();
    }
    return false;
}


static jboolean nRestoreToCount(JNIEnv* env, jclass,  jlong handle, jint count) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    if(graphics) {
        return graphics->restoreToCount(count);
    }
    return false;
}


static void nDrawChar(JNIEnv* env, jclass, jlong handle, jchar c, jint x, jint y, jint anchor, jlong fontHandle) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    auto* font = reinterpret_cast<const Font*>(fontHandle);
    if(graphics != nullptr && font != nullptr) {
        graphics->drawChar((char16_t )c, x, y, anchor, *font);
    }
}


 extern "C" int register_m2g_Graphics(JNIEnv *env) {
#define BIND_JNI(fun, sig) { const_cast<char*>(#fun), const_cast<char*>(sig), reinterpret_cast<void*>(fun)}
     static const JNINativeMethod methods[] = {
             BIND_JNI(nDrawChar, "(JCIIIJ)V"),
             BIND_JNI(nCreateFormImage, "(J)J"),
             BIND_JNI(nRelease, "(J)V"),
             BIND_JNI(nDrawLine, "(JIIII)V"),
             BIND_JNI(nDrawRect, "(JIIII)V"),
             BIND_JNI(nFillRect, "(JIIII)V"),
             BIND_JNI(nDrawRoundRect, "(JIIIIII)V"),
             BIND_JNI(nFillRoundRect, "(JIIIIII)V"),
             BIND_JNI(nDrawArc, "(JIIIIII)V"),
             BIND_JNI(nFillArc, "(JIIIIII)V"),
             BIND_JNI(nDrawCircle, "(JIII)V"),
             BIND_JNI(nFillCircle, "(JIII)V"),
             BIND_JNI(nDrawImage, "(JJIII)V"),
             BIND_JNI(nDrawImageWH, "(JJIIIII)V"),
             BIND_JNI(nDrawRegion, "(JJIIIIIIII)V"),
             BIND_JNI(nDrawString, "(JLjava/lang/String;IIIJ)V"),
             BIND_JNI(nCopyArea, "(JIIIIIII)V"),
             BIND_JNI(nDrawRGB, "(J[IIIIIIIZ)V"),
             BIND_JNI(nSave, "(J)I"),
             BIND_JNI(nRestore, "(J)Z"),
             BIND_JNI(nRestoreToCount, "(JI)Z"),
             BIND_JNI(nSetColor, "(JI)V"),
             BIND_JNI(nTranslate, "(JII)V"),
             BIND_JNI(nGetTranslateX, "(J)I"),
             BIND_JNI(nGetTranslateY, "(J)I"),
             BIND_JNI(nSetClip, "(JIIII)V"),
             BIND_JNI(nClipRect, "(JIIII)V"),
             BIND_JNI(nGetClipX, "(J)I"),
             BIND_JNI(nGetClipY, "(J)I"),
             BIND_JNI(nGetClipWidth, "(J)I"),
             BIND_JNI(nGetClipHeight, "(J)I"),
     };




     const auto clazz = jniFindClass(env, "iml/m2g/NativeGraphics");
     return clazz
            ? jniRegisterNatives(env, clazz, methods, std::size(methods))
            : JNI_ERR;
 }