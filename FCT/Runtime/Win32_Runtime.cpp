#include "../FCTAPI.h"

namespace FCT {
	struct Win32_RuntimeCommon : public RuntimeCommon {
	
	};
	void Runtime::init() {
		g_common = FCT_NEW(Win32_RuntimeCommon);
	}
	void Runtime::term() {
	    FCT_DELETE(g_common);
	}

	Window* Runtime::createWindow()
	{
#ifdef FCT_USE_GLFW
		Window* window = FCT_NEW(GLFW_Window,g_common->glfwUICommon,this);
		return window;
#endif
	}

	Context* Runtime::createContext()
	{
        return FCT_NEW(VK_Context,g_common->vkContextCommon);
	}

	ImageLoader* Runtime::createImageLoader()
	{
		return FCT_NEW(FreeImage_ImageLoader);
	}

}
