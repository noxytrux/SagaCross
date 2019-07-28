#include "native-lib.h"

#include <stdint.h>
#include <string>

JNIEXPORT jstring JNICALL Java_com_noclip_marcinmalysz_sagacross_MainActivity_stringFromJNI( JNIEnv* env, jobject /* this */) {

    std::string hello = "Hello from C++";

    return env->NewStringUTF(hello.c_str());
}
