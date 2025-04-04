#pragma once
#include "../MutilThreadBase/RefCount.h"
#include "../Context/DataTypes.h"
#include "../Context/IRenderTarget.h"
#include "./EventHandler.h"
#include "./CallBackHandler.h"
#include "../Context/ImageRenderTarget.h"
#include "../RHI/Semaphore.h"

namespace FCT {
    class WindowBehavior {
    public:
        virtual void pos(int x,int y) = 0;
        virtual void size(int w, int h) = 0;
    private:

    };
    class SetParamWindowBehavior;
	class EventHandler;
    using VFuncII = std::function<void(int,int)>;
	class Window : public RefCount,public IRenderTarget {
    private:
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
		virtual ImageRenderTarget* getCurrentTarget() = 0;
		virtual RHI::Semaphore* getImageAvailableSemaphore() = 0;
		virtual void addRenderFinshSemaphore(RHI::Semaphore* semaphore) = 0;

	private:
	protected:
		CallBackEventHandler* m_callbackHandler;
		std::vector<EventHandler*> m_handlers;
        int m_x,m_y, m_width, m_height;
		std::string m_title;
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
    };
    inline FCT::Window::Window() {
        m_behavior = new SetParamWindowBehavior(this);
        m_callbackHandler = new CallBackEventHandler();
        registerHandler(m_callbackHandler);
    }
}
