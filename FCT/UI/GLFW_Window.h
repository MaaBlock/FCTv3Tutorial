//
// Created by Administrator on 2025/3/22.
//
#include "../ThirdParty.h"
#include "./Window.h"
#include "../Runtime/Runtime.h"
#ifndef GLFW_WINDOW_H
#define GLFW_WINDOW_H

namespace FCT
{
    class GLFW_WindowBehavior;
    class GLFW_Window : public Window
    {
    public:
        friend class GLFW_WindowBehavior;
        GLFW_Window(GLFW_UICommon* common, Runtime* rt);
        ~GLFW_Window();
        void invokeResizeCallbacks(int width, int height);
        void invokeMouseMoveCallbacks(int xpos, int ypos);
        void invokeMouseCallbacks(int button, int action, int mods);
        void invokeKeyCallbacks(int key, int scancode, int action, int mods);
        void invokeScrollCallbacks(int xoffset, int yoffset);
        void create();
        bool isRunning() const override;
        void bind(Context* ctx) override;
        void swapBuffers() override;
        int getWidth() override;
        int getHeight() override;
        void setCursorPos(int x, int y) override;
        Image* getImage() const override;
    private:
        GLFWwindow* m_window;
        Runtime* m_rt;
        GLFW_UICommon* m_common;
        Context* m_ctx;
        void recreateSwapchain(int width, int height);
#ifdef FCT_USE_VULKAN
        VkSurfaceKHR m_vkSurface;
#endif
        void* m_swapchainNativeHandle;
    };

    class GLFW_UICommon;


    class GLFW_WindowBehavior : public WindowBehavior {
    public:
        GLFW_WindowBehavior(GLFW_Window* wnd) : m_wnd(wnd) {}
        void pos(int x,int y)
        {

        }
        void size(int w, int h)
        {

        }
        void title(const std::string& title)
        {
            m_wnd->m_common->postUiTask([this, title](void*)
            {
                glfwSetWindowTitle(m_wnd->m_window, title.c_str());
            },nullptr,false);
        }
    private:
        GLFW_Window* m_wnd;
    };
}

#endif //GLFW_WINDOW_H
