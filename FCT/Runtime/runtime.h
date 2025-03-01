#pragma once
#include "../UI/Window.h"
#include "../UI/Android_WindowShareData.h"
#include "../ImageLoader/ImageLoader.h"
namespace FCT
{
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
    private:
        RuntimeCommon* m_commmon;
        bool m_isTerm;
        bool m_isRelease;
        void init();
        void term();
    };
#ifdef FCT_DEPRECATED
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