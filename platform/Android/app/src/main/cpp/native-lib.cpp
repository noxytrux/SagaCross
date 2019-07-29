#include "native-lib.h"

#include <stdint.h>

#include "SCApplication.h"
#include "SCMobileInput.h"

using namespace sc;

std::shared_ptr<SCApplication> application = nullptr;
std::shared_ptr<SCSettings> settings = nullptr;

std::string jstringTostring(JNIEnv *env, jstring jStr){
    const char *cstr = env->GetStringUTFChars(jStr, NULL);
    std::string str = std::string(cstr);
    env->ReleaseStringUTFChars(jStr, cstr);
    return str;
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_initializeEngine(JNIEnv* env, jobject object, jint width, jint height, jstring path)
{
    std::string rootPath = jstringTostring(env, path);

    rootPath += "/resource/";

    std::string settingsPath = rootPath + "settings.bin";

    settings = std::make_shared<SCSettings>(settingsPath);

    if (!settings->load()) {

        settings->save();
    }

    settings->setWidht(width);
    settings->setHeight(height);

    application = std::make_shared<SCApplication>(settings, rootPath);
    application->setShouldUseRunLoop(false);
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_renderGame(JNIEnv *env, jobject object)
{
    if (!application) {
        return;
    }

    application->run();
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_handleTouch(JNIEnv *env, jobject object, jfloat x, jfloat y, jboolean selected)
{
    if (!application) {
        return;
    }

    auto input = std::dynamic_pointer_cast<SCMobileInput>(application->getInput());

    input->mousePos = xVec2(x, y);
    input->selected = selected;
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_handleMovement(JNIEnv *env, jobject object, jfloat dx, jfloat dy, jfloat angle)
{
    if (!application) {
        return;
    }

    auto input = std::dynamic_pointer_cast<SCMobileInput>(application->getInput());

    input->movementCallback(xVec2(dx, dy), angle);
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_fireBullet(JNIEnv *env, jobject object)
{
    if (!application) {
        return;
    }

    auto input = std::dynamic_pointer_cast<SCMobileInput>(application->getInput());

    input->mouseCallback(0, 1, xVec2(0));
}

JNIEXPORT void JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_dropMine(JNIEnv *env, jobject object)
{
    if (!application) {
        return;
    }

    auto input = std::dynamic_pointer_cast<SCMobileInput>(application->getInput());

    input->mouseCallback(1, 1, xVec2(0));
}

JNIEXPORT jboolean JNICALL Java_com_noclip_marcinmalysz_sagacross_SCGameWrapper_renderingGame(JNIEnv *env, jobject object)
{
    if (!application) {
        return false;
    }

    return application->isRenderingGame();
}

