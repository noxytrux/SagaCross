#include "SCApplication.h"

#ifdef _WIN32
#pragma comment(lib, "fmod_vc")
#pragma comment(lib, "fmodL_vc")
#pragma comment(lib, "glew32")
#pragma comment(lib, "glew32s")
#pragma comment(lib, "glfw3")
#pragma comment(lib, "glfw3dll")
#pragma comment(lib, "opengl32")
#pragma comment(lib, "glu32")

//TODO: multiplayer
//#pragma comment(lib, "Ws2_32.lib")
//#pragma comment(lib, "Mswsock.lib")
//#pragma comment(lib, "AdvApi32.lib")
#endif

using namespace sc;

std::string resourcePath() {

#if defined(__linux__)

    char result[PATH_MAX];

    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string path = std::string(result, (count > 0) ? count : 0);

    return path.substr(0, path.find_last_of("\\/")) + "/resource/";

#else

    return "resource/";

#endif

}

int main()
{
    auto filename = resourcePath() + "settings.bin";
    auto settings = std::make_shared<SCSettings>(filename);

    if (!settings->load()) {

        settings->save();
    }

	auto app = std::make_unique<SCApplication>(settings, resourcePath());

	return app->run();
}

