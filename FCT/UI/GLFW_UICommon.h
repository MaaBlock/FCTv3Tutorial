//
// Created by Administrator on 2025/3/22.
//
#include "../ThirdParty.h"
#ifndef GLFW_UICOMMON_H
#define GLFW_UICOMMON_H
namespace FCT{

    using UITaskFunction = std::function<void(void *)>;
    struct UiTaskData {
        UITaskFunction task;
        void *param;
        std::shared_ptr<bool> waiting;
    };
	struct UITaskTrans
	{
	    UITaskTrans() = default;
		UITaskTrans(UiTaskData* data) : data(data) {}
		UiTaskData* data;
	};
	using UiTicker = std::function<void(void)>;

	class GLFW_UICommon {
	public:
		GLFW_UICommon();
  		~GLFW_UICommon();
        void postUiTask(UITaskFunction task,void* param = nullptr,bool waited = true);
		uint32_t postTicker(UiTicker ticker)
		{
			uint32_t id = ++m_tickerId;
			postUiTask([this,ticker,id](void*)
			{
				m_ticker[id] = ticker;
			});
			return id;
		}
		void sendUiTask(UITaskFunction task,void* param = nullptr,bool waited = true)
		{
			if (m_uiThreadId == std::this_thread::get_id())
			{
				task(param);
			} else
			{
				postUiTask(task, param, waited);
			}
		}
	private:
		std::thread m_uiThread;
		bool m_running;
		boost::lockfree::queue<UITaskTrans, boost::lockfree::capacity<1024>> m_taskQueue;
		std::unordered_map<uint32_t,UiTicker> m_ticker;
		uint32_t m_tickerId = 0;
		std::thread::id m_uiThreadId;
	};

}
#endif //GLFW_UICOMMON_H
