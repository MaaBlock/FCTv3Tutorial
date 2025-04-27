//
// Created by Administrator on 2025/3/22.
//
#include "../FCTAPI.h"
namespace FCT
{
    GLFW_UICommon::GLFW_UICommon()
    {
        m_running = true;
        bool inited = false;
        m_uiThread = std::thread([this,&inited]()
        {
            m_uiThreadId = std::this_thread::get_id();
            glfwInit();
            glfwSwapInterval(0);
            inited = true;
            while (m_running)
            {
                while (!m_taskQueue.empty() && m_running) {
                    UITaskTrans trans;
                    m_taskQueue.pop(trans);
                    UiTaskData* task = trans.data;
                    task->task(task->param);
                    *task->waiting = false;
                    delete task;
                }
                for (auto it : m_ticker)
                {
                    it.second.operator()();
                }
                glfwPollEvents();
            }
            glfwTerminate();
        });
        while (!inited)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    GLFW_UICommon::~GLFW_UICommon()
    {
        m_running = false;
        m_uiThread.join();
    }

    void GLFW_UICommon::postUiTask(UITaskFunction task, void* param, bool waited)
    {
        UiTaskData* data = new UiTaskData();
        std::shared_ptr<bool> waiting = std::make_shared<bool>(waited);
        data->task = task;
        data->param = param;
        data->waiting = waiting;
        m_taskQueue.push(UITaskTrans(data));
        while (*waiting) {
            std::this_thread::sleep_for(std::chrono::milliseconds(0));
        }
    }

}
