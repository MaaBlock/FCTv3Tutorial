#pragma once
#include "../ThirdParty.h"
#include "./Window.h"
#include "../Runtime/runtime.h"

namespace FCT {

#ifdef FCT_DEPRECATED
	using UITaskFunction = std::function<void(void*)>;
	struct Android_UiTaskData {
		UITaskFunction func;
		void* param;
		std::shared_ptr<bool> waited;
	};
	struct Android_UiTask {
		void (*func)(Android_UiTaskData*);
		Android_UiTaskData* data;
	};
	class Android_WindowShareData {
	public:
		Android_WindowShareData(Runtime* runtime);
		std::thread* getUiThread();
		void init();
		void postUiTask(UITaskFunction func, void* param = nullptr, bool waited = true);
		Window* createWindow(int x, int y, const char* title);
	private:
		Runtime* g_runtime;
		boost::lockfree::queue<Android_UiTask> g_taskQueue;
		std::thread* g_uiThread;
		bool g_inited;
		bool g_runing;
	};
#endif
}