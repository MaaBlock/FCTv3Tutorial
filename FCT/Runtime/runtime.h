#pragma once
#include "../ThirdParty.h"
#include "../UI/Window.h"
#include "../UI/Android_WindowShareData.h"
#include "../ImageLoader/ImageLoader.h"
#include "../Context/VK_ContextCommon.h"
#include "../ModelLoader/ModelLoader.h"
#include "../UI/GLFW_UICommon.h"
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
            g_common->vkContextCommon = new VK_ContextCommon(this);
            g_common->vkContextCommon->init();
#endif
#ifdef  FCT_USE_GLFW
        	g_common->glfwUICommon = new GLFW_UICommon();
#endif
#ifdef FCT_USE_FREEIMAGE
        	//todo:从FCTv2 直接迁移过来 的，应当改为common的模式
			FreeImage_ImageLoader::Init();
#endif
        }
        ~Runtime() {
#ifdef  FCT_USE_VULKAN
            g_common->vkContextCommon->term();
#endif
        	term();
#ifdef  FCT_USE_GLFW
        	delete g_common->glfwUICommon;
#endif
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
    private:
        RuntimeCommon* g_common;
    	Runtime(const Runtime&) = delete;
    	Runtime& operator=(const Runtime&) = delete;
    };
#ifdef FCT_DEPRECATED
    struct RuntimeCommon {

    };
    class Runtime {
    public:
        Runtime()
        {
            init();
        }
        ~Runtime()
        {
            term();
        }
        Window *createWindow(int w, int h, const char *title);
        Context* createContext();
    private:
        RuntimeCommon* m_common;
        bool m_isTerm;
        bool m_isRelease;
        void init();
        void term();
    };
	class Android_WindowShareData;
	class GL_ContextShareData;
	class FreeType_FontShareData;
	class Pipeline;
	class PhysicsSystem;
	class Runtime
	{
	public:
		Runtime()
		{
			init();
		}
		~Runtime()
		{
			m_isRelease = true;
			if (!m_isTern)
				term();
		}
		Runtime(Runtime &) = delete;
		Runtime &operator=(const Runtime &) = delete;
		Runtime(Runtime &&) = delete;
		Runtime &operator=(Runtime &&) = delete;
		void init();
		void term();
		Window *createWindow(int w, int h, const char *title);
		Context *createContext(IRenderTarget *target);
		void setOpenGLVesion(int major, int minor);
		//Font *createFont();
		ImageLoader *createImageLoader();
		Pipeline *createVectorRenderPipeline(Context *ctx);

        //PhysicsSystem* createPhysicsSystem();
	private:
		bool m_isRelease = false;
		bool m_isTern = false;
		/*Android_WindowShareData *g_glfwWindowShareData;
		GL_ContextShareData *g_glContextShareData;
		FreeType_FontShareData *g_freeTypeFontShareData;
		PX_PhysicsShareData* g_phsyShareData;
	*/};
	Runtime *CreateRuntime();
#endif
}