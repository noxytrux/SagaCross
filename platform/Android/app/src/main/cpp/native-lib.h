#ifndef NATIVE_LIB_H
#define NATIVE_LIB_H

#include <jni.h>

extern "C" {

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_initializeEngine(JNIEnv* env, jobject object, jint width, jint height, jstring path);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_renderGame(JNIEnv *env, jobject object);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_handleTouch(JNIEnv *env, jobject object, jfloat x, jfloat y, jboolean selected);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_handleMovement(JNIEnv *env, jobject object, jfloat dx, jfloat dy, jfloat angle);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_fireBullet(JNIEnv *env, jobject object);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_dropMine(JNIEnv *env, jobject object);
JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_renderingGame(JNIEnv *env, jobject object);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_mute(JNIEnv *env, jobject object);
JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_unmute(JNIEnv *env, jobject object);

}

#endif