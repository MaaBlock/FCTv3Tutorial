#pragma once
#include "../MutilThreadBase/RefCount.h"
#include "../Context/IRenderTarget.h"
#include "./EventHandler.h"
#include "./CallBackHandler.h"
namespace FCT {
	class EventHandler;
	class Window : public RefCount,public IRenderTarget {
	public:
		Window() {
			m_callbackHandler = new CallBackEventHandler();
			registerHandler(m_callbackHandler);
		}
		virtual ~Window() {
			clearHandler();
			m_callbackHandler->release();
		}
		virtual bool isRunning() const = 0;
		virtual void swapBuffers() = 0;
		virtual int getWidth() = 0;
		virtual int getHeight() = 0;
		virtual void setCursorPos(int x, int y) = 0;
		void registerHandler(EventHandler* handler);
		void unregisterHandler(EventHandler* handler);
		void clearHandler();
		CallBackEventHandler* getCallBack() const {
			return m_callbackHandler;
		}
	private:
	protected:
		CallBackEventHandler* m_callbackHandler;
		std::vector<EventHandler*> m_handlers;
	private:

	};
}