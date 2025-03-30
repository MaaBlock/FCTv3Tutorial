//
// Created by Administrator on 2025/2/20.
//
#include "../ThirdParty.h"
#include "Window.h"
#include "../Runtime/ActivityWrapper.h"
#ifdef FCT_USE_VULKAN
#include "../Context/VK_Swapchain.h"
#endif

#ifndef ANDROID_WINDOW_H
#define ANDROID_WINDOW_H
namespace FCT {
    class Android_Window : public Window {
    public:
        Android_Window();
        Android_Window(JavaVM* vm);

        void bind(FCT::Context *ctx) override;

        bool isRunning() const override;

        void swapBuffers() override;

        int getWidth() override;

        int getHeight() override;
        uint32_t getDeviceWidth();
        uint32_t getDeviceHeight();
        void onDestory();

        void setCursorPos(int x, int y) override;

        void invokeResizeCallbacks(int width, int height);

        void invokeMouseMoveCallbacks(int xpos, int ypos);

        void invokeMouseCallbacks(int button, int action, int mods);

        void invokeKeyCallbacks(int key, int scancode, int action, int mods);

        void invokeScrollCallbacks(int xoffset, int yoffset);

        void create();

        void destroy();

        bool isCreated() const;

        bool hasSurface() const;

        //void createSurface(jobject surface);

        void destroySurface();
        void create(float w,float h,const char* title);

        void viewport(Vec2 lt, Vec2 rb);
        void sizeParam(int w,int h);
        void resize(int w,int h);
        void onSurfaceCreate(jobject surface);
        void onSurfaceChanged(int width, int height);
        void onSurfaceDestroy();
    private:
        jni::GlobalObject<FCTWindowJavaClass>* m_windowObject;
        Context* m_ctx;
        JNIEnv* getEnv(){
            JNIEnv* env = nullptr;
            m_vm->GetEnv((void **)&env, JNI_VERSION_1_6);
            return env;
        }
        JavaVM* m_vm;
        bool m_hasSurface;
        bool m_running;
        bool m_created;
        const char *m_title;
        Vec2 m_lt;
        Vec2 m_rb;
        void* m_contextReource;
        ANativeWindow* m_nativeWindow;
        bool m_needRecreateSwapChain;
#ifdef FCT_USE_VULKAN
        VK_Swapchain* m_vkSwapchain;

        void createVulkanSurface();
        void destroyVulkanSurface();
        void createVulkanSwapChain();
        void presentVulkan();
        vk::SurfaceKHR m_vulkanSurface;
        //vk::SwapchainKHR m_vulkanSwapchain;

#endif
        //Image* m_buffer;
        std::mutex m_nativeSurfaceMutex;
        std::function<void()> destorySurface;
        std::function<void()> createSurface;
        std::function<void()> recreateSwapChain;
        std::function<void()> present;
    };
}

#endif //ANDROID_WINDOW_H
