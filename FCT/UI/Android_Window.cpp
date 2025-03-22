#include "../FCTAPI.h"
#include "Android_Window.h"


namespace FCT{

#ifdef FCT_USE_VULKAN

    struct Vulkan_WndResource {
        vk::Image image;
    };
#endif
#ifdef FCT_USE_VULKAN

#endif
    void Android_Window::bind(FCT::Context *srcCtx) {
        m_ctx = srcCtx;
        m_ctx->setFlushWindow(this);
        if (false) {

        }
#ifdef FCT_USE_VULKAN
        else if (dynamic_cast<VK_Context*>(srcCtx)) {
            auto ctx = dynamic_cast<VK_Context *>(srcCtx);
            bool needReWaited;
            do {
                needReWaited = false;
                while (!hasSurface()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                m_nativeSurfaceMutex.lock();
                if (!hasSurface()) {
                    needReWaited = true;
                    m_nativeSurfaceMutex.unlock();
                    continue;
                }
                createVulkanSurface();
                createVulkanSwapChain();
                m_needRecreateSwapChain = false;
                //m_vkSwapchain = new VK_Swapchain(ctx);
                m_contextReource = new Vulkan_WndResource();
                recreateSwapChain = std::bind(&Android_Window::createVulkanSwapChain, this);
                present = std::bind(&Android_Window::presentVulkan, this);
                destorySurface = std::bind(&Android_Window::destroyVulkanSurface, this);
                createSurface = std::bind(&Android_Window::createVulkanSurface, this);

                m_nativeSurfaceMutex.unlock();

            } while (needReWaited);
        }
            /*
            else if (dynamic_cast<VK_Context*>(srcCtx)) {
                auto ctx = dynamic_cast<VK_Context*>(srcCtx);
                bool needReWaited;
                do {
                    needReWaited = false;
                    while (!hasSurface()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                    m_nativeSurfaceMutex.lock();
                    if (!hasSurface()) {
                        needReWaited = true;
                        m_nativeSurfaceMutex.unlock();
                        continue;
                    }
                    auto w =  ANativeWindow_getWidth(m_nativeWindow);
                    auto h = ANativeWindow_getHeight(m_nativeWindow);
                    m_buffer = ctx->createImage();
                    m_buffer->create(w,h);
                    m_buffer->initTarget();
                    m_nativeSurfaceMutex.unlock();
                } while (needReWaited);
                m_contextReource = new Vulkan_WndResource();
                recreateSwapChain = std::bind(&Android_Window::createVulkanSwapChain, this);
                present = std::bind(&Android_Window::presentVulkan, this);
            }
             */
#endif
        else {
            ferr << "没有受支持的context" << std::endl;
        }
    }

    bool Android_Window::isRunning() const {
        return m_running;
    }

    void Android_Window::swapBuffers() {
        auto ctx = m_ctx;
        bool needReWaited;
        do {
            needReWaited = false;
            while (!hasSurface()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            m_nativeSurfaceMutex.lock();
            if (!hasSurface()) {
                needReWaited = true;
                m_nativeSurfaceMutex.unlock();
                continue;
            }
            if (m_needRecreateSwapChain) {
                createSurface();
                recreateSwapChain();
                m_needRecreateSwapChain = false;
            }
            present();
            m_nativeSurfaceMutex.unlock();
        } while (needReWaited);
    }

    int Android_Window::getWidth() {
        return 0;
    }

    int Android_Window::getHeight() {
        return 0;
    }

    void Android_Window::setCursorPos(int x, int y) {

    }

    void Android_Window::create() {
        g_androidRuntime->getUICommon()->postUiTask([this](void* ){
            jni::GlobalObject<FCTMainActivityJavaClass>* activity = (g_androidRuntime->getActivity());
            m_windowObject = new jni::GlobalObject<FCTWindowJavaClass>(
                    activity->Call<"createWindow">(jlong(this),m_x,m_y,m_width,m_height));
            aout << "创建窗口" << std::endl;
        });
        m_created = true;
        m_running = true;
    }

    void Android_Window::destroy() {
        m_running = false;
    }

    bool Android_Window::isCreated() const {
        return m_created;
    }
/*
    void Android_Window::createSurface(jobject surface) {
        if (m_nativeWindow) {
            ANativeWindow_release(m_nativeWindow);
        }
        m_nativeWindow = ANativeWindow_fromSurface(getEnv(), surface);
        m_needRecreateSwapChain = true;
        m_hasSurface = true;
    }
*/
    void Android_Window::destroySurface() {
        m_hasSurface = false;
        m_nativeSurfaceMutex.lock();
        if (m_nativeWindow) {
            ANativeWindow_release(m_nativeWindow);
            m_nativeWindow = nullptr;
        }
        m_nativeSurfaceMutex.unlock();
    }

    void Android_Window::viewport(Vec2 lt, Vec2 rb) {
        m_lt = lt;
        m_rb = rb;
    }

    void Android_Window::create(float w, float h, const char *title) {
        viewport(Vec2(0, 0), Vec2(w, h));
        m_title = title;;
    }

    uint32_t Android_Window::getDeviceWidth() {

        return 0;
    }

    uint32_t Android_Window::getDeviceHeight() {
        return 0;
    }

    Android_Window::Android_Window(JavaVM *vm) {
        m_created = false;
        m_running = false;
        m_hasSurface = false;
        m_nativeWindow = nullptr;
        m_vm = vm;
        m_needRecreateSwapChain = true;
    }
/*
    void Android_Window::createVulkanSurface() {
        if (m_vulkanSurface) {
            auto vkInstance = g_AndroidRuntime.getVKContextCommon()->getInstance();
            vkInstance.destroySurfaceKHR(m_vulkanSurface);
            m_vulkanSurface = nullptr;
        }

        vk::AndroidSurfaceCreateInfoKHR createInfo{};
        createInfo.setWindow(m_nativeWindow);
        auto vkInstance = g_AndroidRuntime.getVKContextCommon()->getInstance();
        m_vulkanSurface = vkInstance.createAndroidSurfaceKHR(createInfo);
    }

    void Android_Window::createVulkanSwapChain() {
        vk::SwapchainCreateInfoKHR createInfo{};
        createVulkanSurface();
        createInfo.setSurface(m_vulkanSurface)
                  .setMinImageCount(2)
                  .setImageFormat(vk::Format::eR8G8B8A8Unorm);
    }
*/
    bool Android_Window::hasSurface() const {
        return m_hasSurface;
    }

    Android_Window::Android_Window() {
        m_x = 0;
        m_y = 0;
        m_width = 800;
        m_height = 600;
        m_vkSwapchain = nullptr;
        m_needRecreateSwapChain = false;
    }

    void Android_Window::sizeParam(int w, int h) {
        m_width = w;
        m_height = h;
    }

    void Android_Window::resize(int w, int h) {

    }

    void Android_Window::onDestory() {
        m_running = false;
    }

    void Android_Window::onSurfaceCreate(jobject surface) {
        m_nativeWindow = ANativeWindow_fromSurface(g_androidRuntime->getEnv(),surface);
        ANativeWindow_acquire(m_nativeWindow);
        fout << "onSurfaceCreate" << std::endl;

        auto w =  ANativeWindow_getWidth(m_nativeWindow);
        auto h = ANativeWindow_getHeight(m_nativeWindow);

        fout << "NativeWindow width: " << w << std::endl;
        fout << "NativeWindow height: " << h << std::endl;

        m_needRecreateSwapChain = true;
        m_hasSurface = true;
    }

    void Android_Window::onSurfaceChanged(int width, int height) {

        fout << "onSurfaceChanged" << std::endl;
    }

    void Android_Window::onSurfaceDestroy() {
        m_nativeSurfaceMutex.lock();
        ANativeWindow_release(m_nativeWindow);
        fout << "onSurfaceDestroy" << std::endl;
        m_hasSurface = false;
        m_nativeSurfaceMutex.unlock();
    }
#ifdef FCT_USE_VULKAN

    void Android_Window::createVulkanSurface() {
        if (m_vulkanSurface) {
            destroyVulkanSurface();
        }
        vk::AndroidSurfaceCreateInfoKHR createInfo;
        createInfo.setWindow(m_nativeWindow);

        auto vkInstance = (dynamic_cast<VK_Context*>(m_ctx))->getVkInstance();
        m_vulkanSurface = vkInstance.createAndroidSurfaceKHR(createInfo);
    }

    void Android_Window::createVulkanSwapChain() {
        if (!m_vkSwapchain)
            m_vkSwapchain = new VK_Swapchain((VK_Context*)m_ctx);
        m_vkSwapchain->create(m_vulkanSurface,m_width,m_height);
    }

    void Android_Window::destroyVulkanSurface() {
        if (m_vulkanSurface) {
            auto vkInstance = (dynamic_cast<VK_Context*>(m_ctx))->getVkInstance();
            vkInstance.destroySurfaceKHR(m_vulkanSurface);
            m_vulkanSurface = nullptr;
        }
    }

    void Android_Window::presentVulkan() {
        if (m_vkSwapchain) {
            m_vkSwapchain->present();
        }
    }


#endif
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_androidtutorial_FCTWindow_onDismiss(JNIEnv *env, jobject thiz) {
    jni::LocalObject<FCTWindowJavaClass> window(thiz);
    FCT::Android_Window* pThis = (FCT::Android_Window*)window.Call<"getNativeWindowPtr">();
    FCT::fout << "窗口关闭" << std::endl;
    pThis->onDestory();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_androidtutorial_FCTSurfaceView_onSUrfaceDestroyed(JNIEnv *env, jobject thiz,
                                                                   jlong native_window_ptr) {
    FCT::Android_Window* wnd = (FCT::Android_Window*)native_window_ptr;
    wnd->onSurfaceDestroy();
    // TODO: implement onSUrfaceDestroyed()
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_androidtutorial_FCTSurfaceView_onSurfaceCreated(JNIEnv *env, jobject thiz,
                                                                 jobject surface,
                                                                 jlong native_window_ptr) {
    // TODO: implement onSurfaceCreated()
    FCT::Android_Window* wnd = (FCT::Android_Window*)native_window_ptr;
    wnd->onSurfaceCreate(surface);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_androidtutorial_FCTSurfaceView_onSurfaceChanged(JNIEnv *env, jobject thiz,
                                                                 jlong native_window_ptr,
                                                                 jint width, jint height) {
    // TODO: implement onSurfaceChanged()
    FCT::Android_Window* wnd = (FCT::Android_Window*)native_window_ptr;
    wnd->onSurfaceChanged(width, height);
}