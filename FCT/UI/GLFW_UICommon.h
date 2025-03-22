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

	class GLFW_UICommon {
	public:
		GLFW_UICommon();
  		~GLFW_UICommon();

        void postUiTask(UITaskFunction task,void* param = nullptr,bool waited = true);
    private:
		std::thread m_uiThread;

	};
}
#endif //GLFW_UICOMMON_H
