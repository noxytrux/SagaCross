#ifndef NATIVE_LIB_H
#define NATIVE_LIB_H

#include <jni.h>

extern "C" {

JNIEXPORT jstring JNICALL Java_com_noclip_marcinmalysz_sagacross_MainActivity_stringFromJNI(JNIEnv* env, jobject /* this */);

}

#endif