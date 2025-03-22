//
// Created by Administrator on 2025/2/27.
//
#pragma once
#include "../ThirdParty.h"

#ifndef ANDROID_UICOMMON_H
#define ANDROID_UICOMMON_H
namespace FCT {
    using UITaskFunction = std::function<void(void *)>;

    struct UiTaskData {
        UITaskFunction task;
        void *param;
        std::shared_ptr<bool> waiting;
    };

    class Android_UICommon {
    public:

        void preinit();
        void cleanup();
        void postUiTask(UITaskFunction task,void* param = nullptr,bool waited = true);
    private:
        void processUiTask(int fd);
        ALooper* m_uiLooper;
        int m_messagePipe[2];
        static int looperWork(int fd, int events, void *data);
    };
}
#endif //ANDROID_UICOMMON_H
