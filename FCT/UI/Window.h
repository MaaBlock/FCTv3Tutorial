#pragma once
#include "../MutilThreadBase/RefCount.h"
#include "../Context/DataTypes.h"
#include "../Context/IRenderTarget.h"
#include "./EventHandler.h"
#include "./CallBackHandler.h"
#include "../Context/ImageRenderTarget.h"
#include "../RHI/Semaphore.h"
#include "../RHI/Swapcain.h"
#include "../DebugTools/OutStream.h"

namespace FCT {
	namespace RHI
	{
		class Swapchain;
	}

	class WindowBehavior
	{
    public:
        virtual void pos(int x,int y) = 0;
        virtual void size(int w, int h) = 0;
		virtual void title(const std::string& title) = 0;
    private:

    };
    class SetParamWindowBehavior;
	class EventHandler;
    using VFuncII = std::function<void(int,int)>;
	class Window : public RefCount,public IRenderTarget {
    protected:
        WindowBehavior* m_behavior;
    public:
        friend class SetParamWindowBehavior;
		RenderTargetType getType() const override { return RenderTargetType::WindowTarget; }
		Window();
		virtual ~Window() {
			clearHandler();
			m_callbackHandler->release();
		}
        void pos(int x, int y) {
            m_behavior->pos(x, y);
        }
        void size(int w, int h) {
            m_behavior->size(w, h);
        }
        virtual void create() = 0;
		virtual bool isRunning() const = 0;
		virtual void swapBuffers() = 0;
		virtual int getWidth() = 0;
		virtual int getHeight() = 0;
        //virtual void viewport(Vec2 lt, Vec2 rb) = 0;
		virtual void setCursorPos(int x, int y) = 0;
		void registerHandler(EventHandler* handler);
		void unregisterHandler(EventHandler* handler);
		void clearHandler();
		CallBackEventHandler* getCallBack() const {
			return m_callbackHandler;
		}
		ImageRenderTarget* getCurrentTarget() {
			return m_swapchain->getCurrentTarget();
		}
		void addRenderFinshSemaphore(RHI::Semaphore* semaphore)
		{
			if (m_swapchain)
			{
				m_swapchain->addRenderFinshSemaphore(semaphore);
				return;
			}
			ferr << "未 bind Context" << std::endl;
		}
		void clearRenderFinshSemaphores()
		{
			m_swapchain->clearRenderFinshSemaphores();
		}
		void setPresentFinshSemaphore(RHI::Semaphore* semaphore)
		{
			m_swapchain->setPresentFinshSemaphore(semaphore);
		}
		void addRenderFinshFence(RHI::Fence* fence)
		{
			m_swapchain->addRenderFinshFence(fence);
		}
		void clearRenderFinshFences()
		{
			m_swapchain->clearRenderFinshFences();
		}
		void initRender();
		void title(const std::string& title)
		{
            m_behavior->title(title);
		}
		//目前只能在bind前 调用,bind后调用为未定义 行为
		void enableDepthBuffer(Format format)
		{
			if (m_swapchain)
			{
				m_swapchain->enableDepthBuffer(format);
				m_needEnableDepthBuffer = false;
			} else
			{
				m_needEnableDepthBuffer = true;
				m_depthBufferFormat = format;
			}
		}
		uint32_t getSwapchainImageCount() const
		{
			return m_swapchain->getImageCount();
		}
		Samples getSwapchainSampleCount() const
		{
			return m_swapchain->getSampleCount();
		}
	private:
	protected:
		CallBackEventHandler* m_callbackHandler;
		std::vector<EventHandler*> m_handlers;
        int m_x,m_y, m_width, m_height;
		std::string m_title;
        RHI::Swapchain* m_swapchain;
		bool m_needEnableDepthBuffer;
		Format m_depthBufferFormat;
	private:

	};

    class SetParamWindowBehavior : public WindowBehavior {
    private:
        Window* m_window;
    public:
        SetParamWindowBehavior(Window* window) : m_window(window) {}
        void pos(int x, int y) override {
            m_window->m_x = x;
            m_window->m_y = y;
        }
        void size(int w, int h) override {
            m_window->m_width = w;
            m_window->m_height = h;
        }
    	void title(const std::string& title) override {
            m_window->m_title = title;
        }
    };
    inline FCT::Window::Window() {
    	m_needEnableDepthBuffer = false;
        m_behavior = new SetParamWindowBehavior(this);
        m_callbackHandler = new CallBackEventHandler();
    	m_callbackHandler->addResizeCallback([this](Window*,int w,int h)
    	{
    		if (m_swapchain)
    		{
    			m_swapchain->size(w,h);
    			m_swapchain->needRecreate();
    		}
    	});
        registerHandler(m_callbackHandler);
    }
}
