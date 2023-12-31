﻿//
// Created by Admin on 2023/5/28.
//

#include "jni_def.h"
#include "JNI_OnLoad.h"
#include "m2g/Image.h"
#include "m2g/Graphics.h"


jlong NativeGraphics_CreateFormImage(JNIEnv *, jclass, jlong handle) {
    if(auto* image = reinterpret_cast<Image*>(handle)) {
        return reinterpret_cast<jlong>(image->getGraphics());
    }
    return 0;
}

void NativeGraphics_Release(JNIEnv *, jclass, jlong handle) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    delete graphics;
}


void NativeGraphics_DrawLine(JNIEnv *, jclass, jlong graphicsHandle, jint x0, jint y0, jint x1, jint y1) {
    if(auto* graphics = reinterpret_cast<Graphics*>(graphicsHandle)) {
        graphics->drawLine(x0, y0, x1, y1);
    }
}

void NativeGraphics_DrawRect(JNIEnv *, jclass, jlong graphicsHandle, jint x, jint y, jint width, jint height) {
    if(auto* graphics = reinterpret_cast<Graphics*>(graphicsHandle)) {
        graphics->drawRect(x, y, width, height);
    }
}

void NativeGraphics_FillRect(JNIEnv *, jclass, jlong graphicsHandle, jint x, jint y, jint width, jint height) {
    if(auto* graphics = reinterpret_cast<Graphics*>(graphicsHandle)) {
        graphics->fillRect(x, y, width, height);
    }
}

void NativeGraphics_DrawCircle(JNIEnv *, jclass, jlong graphicsHandle, jint centerX, jint centerY, jint r) {
    if(auto* graphics = reinterpret_cast<Graphics*>(graphicsHandle)) {
        graphics->drawCircle( centerX, centerY, r);
    }
}

void NativeGraphics_FillCircle(JNIEnv *, jclass, jlong graphicsHandle, jint centerX, jint centerY, jint r) {
    if(auto* graphics = reinterpret_cast<Graphics*>(graphicsHandle)) {
        graphics->fillCircle(centerX, centerY, r);
    }
}


void NativeGraphics_DrawImageWH(JNIEnv *, jclass, jlong graphicsHandle, jlong imageHandle, jint x, jint y, jint width, jint height, jint anchor) {
    auto* graphics = reinterpret_cast<Graphics*>(graphicsHandle);
    auto* image = reinterpret_cast<Image*>(imageHandle);
    if(graphics != nullptr && image != nullptr) {
        graphics->drawImage(image, x, y, width, height, anchor);
    }
}



void NativeGraphics_DrawImage(JNIEnv *, jclass, jlong graphicsHandle, jlong imageHandle, jint x, jint y, jint anchor) {
    auto* graphics = reinterpret_cast<Graphics*>(graphicsHandle);
    auto* image = reinterpret_cast<Image*>(imageHandle);
    if(graphics != nullptr && image != nullptr) {
        graphics->drawImage(image, x, y, anchor);
    }
}


void NativeGraphics_SetColor(JNIEnv *, jclass, jlong handle, jint color) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->setColor(color);
    }
}


void NativeGraphics_Translate(JNIEnv *, jclass, jlong handle, jint x, jint y) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->translate(x, y);
    }
}

void NativeGraphics_SetClip(JNIEnv *, jclass, jlong handle, jint x, jint y, jint width, jint height) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->setClip(x, y, width, height);
    }
}

void NativeGraphics_ClipRect(JNIEnv *, jclass, jlong handle, jint x, jint y, jint width, jint height) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        graphics->clipRect(x, y, width, height);
    }
}

jint NativeGraphics_GetTranslateX(JNIEnv *, jclass, jlong handle) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        return graphics->getTranslateX();
    }
    return 0;
}

jint NativeGraphics_GetTranslateY(JNIEnv *, jclass, jlong handle) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        return graphics->getTranslateY();
    }
    return 0;
}


jint NativeGraphics_GetClipX(JNIEnv *, jclass, jlong handle) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        return graphics->getClipX();
    }
    return 0;
}

jint NativeGraphics_GetClipY(JNIEnv *, jclass, jlong handle) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        return graphics->getClipY();
    }
    return 0;
}



jint NativeGraphics_GetClipWidth(JNIEnv *, jclass, jlong handle) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        return graphics->getClipWidth();
    }
    return 0;
}


jint NativeGraphics_GetClipHeight(JNIEnv *, jclass, jlong handle) {
    if(auto* graphics = reinterpret_cast<Graphics*>(handle)) {
        return graphics->getClipHeight();
    }
    return 0;
}


void NativeGraphics_DrawString(JNIEnv* env, jclass,  jlong handle, jstring str, jint x, jint y, jint anchor, jlong  fontHandle) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    auto* font = reinterpret_cast<const Font*>(fontHandle);
    if(graphics != nullptr && font != nullptr) {
        const char* text = jniGetStringUTFChars(env, str, nullptr);
        graphics->drawString(text, x, y, anchor, *font);
        jniReleaseStringUTFChars(env, str, text);
    }
}

void NativeGraphics_DrawRegion(JNIEnv* env, jclass,  jlong handle, jlong imageHandle,
                                    jint x, jint y, jint width, jint height,
                                    jint transform,  jint x_dst, jint y_dst,
                                    jint anchor) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    auto* image = reinterpret_cast<Image*>(imageHandle);
    if(graphics) {
        graphics->drawRegion(image, x, y, width, height, transform, x_dst, y_dst, anchor);
    }
}

void NativeGraphics_CopyArea(JNIEnv* env, jclass,  jlong handle, jint x_src, jint y_src, jint width, jint height,
                             jint x_dst, jint y_dst, jint anchor) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    if(graphics) {
        graphics->copyArea(x_src, y_src, width, height, x_dst, y_dst, anchor);
    }
}


void NativeGraphics_DrawRGB(JNIEnv* env, jclass,  jlong handle, jintArray array, jint offset, jint scanLength,
                            jint x, jint y, jint width, jint height, jboolean processAlpha) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    if(graphics) {
        auto* data = jniGetIntArrayElements(env, array, nullptr);
        int dataLength = jniGetArrayLength(env, array);
        graphics->drawRGB(data,dataLength, offset, scanLength, x, y, width, height, processAlpha);
        jniReleaseIntArrayElements(env, array, data, JNI_ABORT);
    }
}


void NativeGraphics_Save(JNIEnv* env, jclass,  jlong handle) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    if(graphics) {
        graphics->save();
    }
}


jboolean NativeGraphics_Restore(JNIEnv* env, jclass,  jlong handle) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    if(graphics) {
        return graphics->restore();
    }
    return false;
}


jboolean NativeGraphics_RestoreToCount(JNIEnv* env, jclass,  jlong handle, jint count) {
    auto* graphics = reinterpret_cast<Graphics*>(handle);
    if(graphics) {
        return graphics->restoreToCount(count);
    }
    return false;
}


extern int register_m2g_Graphics(JNIEnv *env) {
    static const JNINativeMethod methods[] = {
            {"jniCreateFormImage"       , "(J)J"   , reinterpret_cast<void*>(NativeGraphics_CreateFormImage)      },
            {"jniRelease"       , "(J)V"           , reinterpret_cast<void*>(NativeGraphics_Release)      },
            {"jniDrawLine"       , "(JIIII)V"           , reinterpret_cast<void*>(NativeGraphics_DrawLine)      },
            {"jniDrawRect"       , "(JIIII)V"           , reinterpret_cast<void*>(NativeGraphics_DrawRect)      },
            {"jniFillRect"       , "(JIIII)V"           , reinterpret_cast<void*>(NativeGraphics_FillRect)      },
            {"jniDrawCircle"       , "(JIII)V"           , reinterpret_cast<void*>(NativeGraphics_DrawCircle)      },
            {"jniFillCircle"       , "(JIII)V"           , reinterpret_cast<void*>(NativeGraphics_FillCircle)      },
            {"jniDrawImage"       , "(JJIII)V"           , reinterpret_cast<void*>(NativeGraphics_DrawImage)      },
            {"jniDrawImageWH"       , "(JJIIIII)V"        , reinterpret_cast<void*>(NativeGraphics_DrawImageWH)      },
            {"jniDrawRegion"       , "(JJIIIIIIII)V"        , reinterpret_cast<void*>(NativeGraphics_DrawRegion)      },
            {"jniDrawString"       , "(JLjava/lang/String;IIIJ)V"    , reinterpret_cast<void*>(NativeGraphics_DrawString)      },
            {"jniCopyArea"       , "(JIIIIIII)V"        , reinterpret_cast<void*>(NativeGraphics_CopyArea)      },
            {"jniDrawRGB"       , "(J[IIIIIIIZ)V"        , reinterpret_cast<void*>(NativeGraphics_DrawRGB)      },
            {"jniSetColor"       , "(JI)V"           , reinterpret_cast<void*>(NativeGraphics_SetColor)      },
            {"jniTranslate"       , "(JII)V"           , reinterpret_cast<void*>(NativeGraphics_Translate)      },
            {"jniGetTranslateX"       , "(J)I"           , reinterpret_cast<void*>(NativeGraphics_GetTranslateX)      },
            {"jniGetTranslateY"       , "(J)I"           , reinterpret_cast<void*>(NativeGraphics_GetTranslateY)      },
            {"jniSetClip"       , "(JIIII)V"           , reinterpret_cast<void*>(NativeGraphics_SetClip)      },
            {"jniClipRect"       , "(JIIII)V"           , reinterpret_cast<void*>(NativeGraphics_ClipRect)      },
            {"jniGetClipX"       , "(J)I"           , reinterpret_cast<void*>(NativeGraphics_GetClipX)      },
            {"jniGetClipY"       , "(J)I"           , reinterpret_cast<void*>(NativeGraphics_GetClipY)      },
            {"jniGetClipWidth"       , "(J)I"           , reinterpret_cast<void*>(NativeGraphics_GetClipWidth)      },
            {"jniGetClipHeight"       , "(J)I"           , reinterpret_cast<void*>(NativeGraphics_GetClipHeight)      },
            {"jniSave"       , "(J)V"           ,      reinterpret_cast<void*>(NativeGraphics_Save)      },
            {"jniRestore"       , "(J)Z"           ,       reinterpret_cast<void*>(NativeGraphics_Restore)      },
            {"jniRestoreToCount"       , "(JI)Z"           , reinterpret_cast<void*>(NativeGraphics_RestoreToCount)      },
    };

    const auto clazz = jniFindClass(env, "iml/m2g/NativeGraphics");
    return clazz
           ? jniRegisterNatives(env, clazz, methods, std::size(methods))
           : JNI_ERR;
}