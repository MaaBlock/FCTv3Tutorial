#ifndef FCT_RUNTIME_H
#define FCT_RUNTIME_H
#include "../ThirdParty.h"
#include "../UI/Window.h"
#include "../UI/Android_WindowShareData.h"
#include "../ImageLoader/ImageLoader.h"
#include "../Context/VK_ContextCommon.h"
#include "../ModelLoader/ModelLoader.h"
#include "../UI/GLFW_UICommon.h"
#include "../VertexAndText/Freetype_FontCommon.h"
#ifdef FCT_USE_FREEIMAGE
        #include "../ImageLoader/FreeImage_ImageLoader.h"
#endif
namespace FCT
{
    struct RuntimeCommon {
#ifdef FCT_USE_VULKAN
        VK_ContextCommon* vkContextCommon;
#endif
#ifdef FCT_USE_GLFW
    	GLFW_UICommon* glfwUICommon;
#endif
    	Freetype_FontCommon* freetypeFontCommon;
    };
	/*
	 *todo:
	 * 此处 runtime 是给用户包装用 的，允许有多个，
	 * 应当在内部统一隐藏一个单例 内部Runtime 负责真正的Runtiem
	 * 对于一些只能调用一次的Init的Init，term
	 */
    class Runtime {
    public:
        Runtime(){
            init();
#ifdef  FCT_USE_VULKAN
            g_common->vkContextCommon = FCT_NEW(VK_ContextCommon,this);
            g_common->vkContextCommon->init();
#endif
#ifdef  FCT_USE_GLFW
        	g_common->glfwUICommon = FCT_NEW(GLFW_UICommon);
#endif
#ifdef FCT_USE_FREEIMAGE
        	//todo:从FCTv2 直接迁移过来 的，应当改为common的模式
			FreeImage_ImageLoader::Init();
#endif
        	g_common->freetypeFontCommon = FCT_NEW(Freetype_FontCommon);
        	g_common->freetypeFontCommon->init();
        }
        ~Runtime() {
#ifdef  FCT_USE_VULKAN
            g_common->vkContextCommon->term();
#endif
        	g_common->freetypeFontCommon->tern();
        	FCT_DELETE(g_common->freetypeFontCommon);
#ifdef  FCT_USE_GLFW
        	FCT_DELETE(g_common->glfwUICommon);
#endif
        	term();
        }
        void init();
        void term();
        int getDeviceWidth();
        int getDeviceHeight();
        Window* createWindow();
        Window* createWindow(int w,int h,const char* title = "") {
            Window* window = createWindow();
            window->size(w,h);
            window->create();
            return window;
        }
        Window* createWindow(int x,int y,int w,int h,const char* title = "") {
            Window* window = createWindow();
            window->pos(x,y);
            window->size(w,h);
            window->create();
            return window;
        }
        Context* createContext();
    	ImageLoader* createImageLoader();
    	ModelLoader* createModelLoader();
    	Freetype_Font* createFont()
    	{
    		return g_common->freetypeFontCommon->create();
    	}
    	void postUiTask(UITaskFunction task,void* param = nullptr,bool waited = true)
    	{

#ifdef  FCT_USE_GLFW
    		g_common->glfwUICommon->postUiTask(task,param, waited);
#endif
    	}
    private:
        RuntimeCommon* g_common;
    	Runtime(const Runtime&) = delete;
    	Runtime& operator=(const Runtime&) = delete;
    };
}
#endif