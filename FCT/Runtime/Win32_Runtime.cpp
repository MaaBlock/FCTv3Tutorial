#include "../FCTAPI.h"

namespace FCT {
	struct Win32_RuntimeCommon : public RuntimeCommon {
	
	};
	void Runtime::init() {
		g_common = new Win32_RuntimeCommon();
	}
	void Runtime::term() {
	    delete g_common;
	}

	Window* Runtime::createWindow()
	{
#ifdef FCT_USE_GLFW
		Window* window = new GLFW_Window(g_common->glfwUICommon,this);
		return window;
#endif
	}

	Context* Runtime::createContext()
	{
        return new VK_Context(g_common->vkContextCommon);
	}

	ImageLoader* Runtime::createImageLoader()
	{
		return new FreeImage_ImageLoader();
	}
}
