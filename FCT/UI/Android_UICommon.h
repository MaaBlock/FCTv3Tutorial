//
// Created by Administrator on 2025/2/27.
//
#pragma once
#include "../ThirdParty.h"
#include "./Window.h"

#ifndef ANDROID_UICOMMON_H
#define ANDROID_UICOMMON_H
using UITaskFunction = std::function<void(void*)>;

struct UiTaskData {
    UITaskFunction func;
    void* param;
    std::shared_ptr<bool> waited;
};

class Android_UICommon {
public:
    void init(ALooper* looper);
    void postUiTask(UITaskFunction func, void* param = nullptr, bool blocked = true);
    FCT::Window* createWindow();
    FCT::Window* createWindow(int w, int h, const char *title);
private:
    static int UiTaskCallback(int fd, int events, void* data);

    ALooper* m_looper;
    int m_pipeFd[2];
};

extern Android_UICommon g_AndroidUiCommon;
#endif //ANDROID_UICOMMON_H
