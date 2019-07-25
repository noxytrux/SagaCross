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

//#pragma comment(lib, "freetype")

//TODO: multiplayer
//#pragma comment(lib, "Ws2_32.lib")
//#pragma comment(lib, "Mswsock.lib")
//#pragma comment(lib, "AdvApi32.lib")
#endif

using namespace sc;

int main()
{
	auto app = std::make_unique<SCApplication>();

	return app->run();
}

