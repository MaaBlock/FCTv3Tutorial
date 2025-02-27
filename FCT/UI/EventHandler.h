#pragma once
#include "../MutilThreadBase/RefCount.h"
#include "../UI/Window.h"
namespace FCT {
	class Window;
	class EventHandler : public RefCount {
	public:
		virtual void onResize(Window* wnd,int width,int height) = 0;
		virtual void onMouseMove(Window* wnd, int xpos, int ypos) = 0;
		virtual void onMouseWheel(Window* wnd,int delta) = 0;
		virtual void onLButtonDown(Window* wnd, int xpos, int ypos) = 0;
		virtual void onLButtonUp(Window* wnd, int xpos, int ypos) = 0;
		virtual void onRButtonDown(Window* wnd, int xpos, int ypos) = 0;
		virtual void onRButtonUp(Window* wnd, int xpos, int ypos) = 0;
		virtual void onKeyDown(Window* wnd, int key) = 0;
		virtual void onKeyUp(Window* wnd, int key) = 0;
	private:

	};
}