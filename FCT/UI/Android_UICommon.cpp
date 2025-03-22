//
// Created by Administrator on 2025/3/10.
//
#include "../FCTAPI.h"
#include "Android_UICommon.h"

namespace FCT {
    void Android_UICommon::preinit() {
        m_uiLooper = ALooper_forThread();
        ALooper_acquire(m_uiLooper);
        if (pipe(m_messagePipe) == -1) {
            ferr << "没有成功创建ui task的pipe" << std::endl;
        }

        ALooper_addFd(m_uiLooper,m_messagePipe[0],ALOOPER_POLL_CALLBACK,
                      ALOOPER_EVENT_INPUT,looperWork, this);
    }

    void Android_UICommon::cleanup() {
        ALooper_removeFd(m_uiLooper,m_messagePipe[0]);
        close(m_messagePipe[0]);
        close(m_messagePipe[1]);
        ALooper_release(m_uiLooper);
    }

    void Android_UICommon::processUiTask(int fd) {
        UiTaskData* wrapper;
        ssize_t size = read(fd, &wrapper, sizeof(UiTaskData*));
        if (size == sizeof(UiTaskData*) && wrapper) {
            if (wrapper->task) {
                wrapper->task(wrapper->param);
            }
            if (wrapper->waiting) {
                *wrapper->waiting = false;
            }
            delete wrapper;
        }
    }

    void Android_UICommon::postUiTask(UITaskFunction task, void *param, bool waited) {
        UiTaskData* data = new UiTaskData();
        std::shared_ptr<bool> waiting = std::make_shared<bool>(waited);
        data->task = std::move(task);
        data->waiting = waiting;
        data->param = param;
        /*todo*/
        write(m_messagePipe[1], &data, sizeof(UiTaskData*));

        while (*waiting) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        fout << "结束" << std::endl;
    }

    int Android_UICommon::looperWork(int fd, int events, void *data) {
        Android_UICommon* common = static_cast<Android_UICommon*>(data);
        common->processUiTask(fd);
        return 1;
    }
}

