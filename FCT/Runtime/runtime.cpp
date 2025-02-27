#include "../headers.h"
namespace FCT
{
    /*
	Runtime *CreateRuntime()
	{
		Runtime *ret = new Runtime;
		return ret;
	}
	void Runtime::init()
	{
		g_glfwWindowShareData = new GLFW_WindowShareData(this);
		g_glfwWindowShareData->init();
		g_glContextShareData = new GL_ContextShareData(this);
		g_glContextShareData->init();
		g_phsyShareData = new PX_PhysicsShareData;
		g_phsyShareData->init();
		FreeImage_ImageLoader::Init();
		g_freeTypeFontShareData = new FreeType_FontShareData;
		g_freeTypeFontShareData->init();
		setOpenGLVesion(3, 2);
	}
	void Runtime::term()
	{
		FreeImage_ImageLoader::Tern();
		g_freeTypeFontShareData->tern();
		g_phsyShareData->term();
		delete g_glfwWindowShareData;
		delete g_glContextShareData;
		delete g_freeTypeFontShareData;
		delete g_phsyShareData;
		m_isTern = true;
		if (!m_isRelease)
		{
			delete this;
		}
	}
	Window *Runtime::createWindow(int w, int h, const char *title)
	{
		return g_glfwWindowShareData->createWindow(w, h, title);
	}
	Context *Runtime::createContext(IRenderTarget *target)
	{
		return g_glContextShareData->createContext(target);
	}
	void Runtime::setOpenGLVesion(int major, int minor)
	{
		g_glContextShareData->setOpenGLVersion(major, minor);
	}
	Font *Runtime::createFont()
	{
		return g_freeTypeFontShareData->create();
	}
	ImageLoader *Runtime::createImageLoader()
	{
		return new FreeImage_ImageLoader();
	}
	Pipeline *Runtime::createVectorRenderPipeline(Context *ctx)
	{
		return nullptr;
	}
	PhysicsSystem* Runtime::createPhysicsSystem()
	{
		PhysicsSystem* ret = new PhysicsSystem(this, g_phsyShareData);
		return ret;
	}*/
}
