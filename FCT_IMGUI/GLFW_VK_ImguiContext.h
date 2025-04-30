//
// Created by Administrator on 2025/4/26.
//
#include "../FCT/FCTAPI.h"
#ifndef VK_GLFW_IMGUICONTEXT_H
#define VK_GLFW_IMGUICONTEXT_H


struct ImGui_ImplGlfw_Data;

namespace FCT
{
    class GLFW_VK_ImGuiContext
    {
    protected:
        RHI::PassGroup* m_passGroup;
        GLFW_Window* m_wnd;
        VK_Context* m_ctx;
   public:
        GLFW_VK_ImGuiContext(GLFW_Window* wnd,VK_Context* ctx);
        void newFrame();
        void submitTick(RHI::CommandBuffer* cmdBuffer);
        void logicTick();
        void create(RHI::PassGroup* passGroup,RHI::Pass* pass)
        {
            m_passGroup = passGroup;
            ImGui_ImplGlfw_InitForVulkan(m_wnd->getWindow(),true);
            ImGui_ImplVulkan_InitInfo init_info{};
            init_info.Instance = m_ctx->getVkInstance();
            init_info.PhysicalDevice = m_ctx->getPhysicalDevice();
            init_info.Device = m_ctx->getDevice();
            init_info.QueueFamily = m_ctx->getGraphicsQueueFamily();
            init_info.Queue = m_ctx->getGraphicsQueue();
            init_info.PipelineCache = nullptr;
            init_info.DescriptorPool = static_cast<RHI::VK_DescriptorPool*>(m_ctx->getDescriptorPool(m_wnd))->getPool();
            init_info.Allocator = nullptr;
            init_info.MinImageCount = 2;
            init_info.ImageCount = m_wnd->getSwapchainImageCount();
            init_info.MSAASamples = static_cast<VkSampleCountFlagBits>(ToVkSampleCount(m_wnd->getSwapchainSampleCount()));
            init_info.CheckVkResultFn = nullptr;
            init_info.RenderPass = static_cast<RHI::VK_PassGroup*>(m_passGroup)->getRenderPass();
            init_info.Subpass = pass->index();
            ImGui_ImplVulkan_Init(&init_info);
            m_wnd->postTicker([this]()
            {
                newFrame_updateInput();
            });
        }
        void updateRenderPass(RHI::PassGroup* passGroup, RHI::Pass* pass)
        {

        }
    protected:
        void newFrame_updataSize();
        void newFrame_UpdateMouseData();
        void newFrame_updataTime();
        void newFrame_UpdateMonitors();
        void newFrame_updateMouseCursor();
        void newFrame_UpdateGamepads();
        void newFrame_updateInput();
    };
    inline GLFW_VK_ImGuiContext::GLFW_VK_ImGuiContext(GLFW_Window* wnd, VK_Context* ctx)
    {
        m_wnd = wnd;
        m_ctx = ctx;
    }



    inline void GLFW_VK_ImGuiContext::newFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        newFrame_updataSize();
        newFrame_UpdateMonitors();
        newFrame_updataTime();
        //newFrame_updateInput();
        ImGui::NewFrame();
    }


    inline void GLFW_VK_ImGuiContext::submitTick(RHI::CommandBuffer* cmdBuffer)
    {

        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();

        VkCommandBuffer vkCmdBuffer = static_cast<RHI::VK_CommandBuffer*>(cmdBuffer)->commandBuffer();
        ImGui_ImplVulkan_RenderDrawData(drawData, vkCmdBuffer);
    }

    inline void GLFW_VK_ImGuiContext::logicTick()
    {
    }

    inline void GLFW_VK_ImGuiContext::newFrame_updataSize()
    {
        ImGuiIO& io = ImGui::GetIO();
        int width, height;
        int display_width, display_height;
        GLFWwindow* window = m_wnd->getWindow();

        glfwGetWindowSize(window, &width, &height);
        glfwGetFramebufferSize(window, &display_width, &display_height);

        io.DisplaySize = ImVec2((float)width, (float)height);

        if (width > 0 && height > 0)
            io.DisplayFramebufferScale = ImVec2((float)display_width / (float)width, (float)display_height / (float)height);
    }
    inline void GLFW_VK_ImGuiContext::newFrame_UpdateMouseData()
    {
        ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
        ImGuiIO& io = ImGui::GetIO();
        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();

        ImGuiID mouse_viewport_id = 0;
        const ImVec2 mouse_pos_prev = io.MousePos;
        for (int n = 0; n < platform_io.Viewports.Size; n++)
        {
            ImGuiViewport* viewport = platform_io.Viewports[n];
            GLFWwindow* window = (GLFWwindow*)viewport->PlatformHandle;

#ifdef EMSCRIPTEN_USE_EMBEDDED_GLFW3
            const bool is_window_focused = true;
#else
            const bool is_window_focused = glfwGetWindowAttrib(window, GLFW_FOCUSED) != 0;
#endif
            if (is_window_focused)
            {
                // (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when io.ConfigNavMoveSetMousePos is enabled by user)
                // When multi-viewports are enabled, all Dear ImGui positions are same as OS positions.
                if (io.WantSetMousePos)
                {
                    m_wnd->sendUiTask([](void* data) {
                        auto params = static_cast<std::tuple<GLFWwindow*, double, double>*>(data);
                        glfwSetCursorPos(std::get<0>(*params), std::get<1>(*params), std::get<2>(*params));
                        delete params;
                    }, new std::tuple<GLFWwindow*, double, double>(
                        window,
                        (double)(mouse_pos_prev.x - viewport->Pos.x),
                        (double)(mouse_pos_prev.y - viewport->Pos.y)
                    ),false);
                }

                // (Optional) Fallback to provide mouse position when focused (ImGui_ImplGlfw_CursorPosCallback already provides this when hovered or captured)
                if (bd->MouseWindow == nullptr)
                {
                    double mouse_x, mouse_y;
                    glfwGetCursorPos(window, &mouse_x, &mouse_y);
                    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                    {
                        // Single viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
                        // Multi-viewport mode: mouse position in OS absolute coordinates (io.MousePos is (0,0) when the mouse is on the upper-left of the primary monitor)
                        int window_x, window_y;
                        glfwGetWindowPos(window, &window_x, &window_y);
                        mouse_x += window_x;
                        mouse_y += window_y;
                    }
                    bd->LastValidMousePos = ImVec2((float)mouse_x, (float)mouse_y);
                    io.AddMousePosEvent((float)mouse_x, (float)mouse_y);
                }
            }

            // (Optional) When using multiple viewports: call io.AddMouseViewportEvent() with the viewport the OS mouse cursor is hovering.
#if GLFW_HAS_MOUSE_PASSTHROUGH
            const bool window_no_input = (viewport->Flags & ImGuiViewportFlags_NoInputs) != 0;

            m_wnd->sendUiTask([](void* data) {
                auto params = static_cast<std::tuple<GLFWwindow*, int, int>*>(data);
                glfwSetWindowAttrib(std::get<0>(*params), std::get<1>(*params), std::get<2>(*params));
                delete params;
            }, new std::tuple<GLFWwindow*, int, int>(
                window,
                GLFW_MOUSE_PASSTHROUGH,
                window_no_input ? GLFW_TRUE : GLFW_FALSE
            ),false);
#endif
#if GLFW_HAS_MOUSE_PASSTHROUGH || GLFW_HAS_WINDOW_HOVERED
            if (glfwGetWindowAttrib(window, GLFW_HOVERED))
                mouse_viewport_id = viewport->ID;
#else
            // We cannot use bd->MouseWindow maintained from CursorEnter/Leave callbacks, because it is locked to the window capturing mouse.
#endif
        }

        if (io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport)
            io.AddMouseViewportEvent(mouse_viewport_id);
    }
    inline void GLFW_VK_ImGuiContext::newFrame_updataTime()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
        double current_time = glfwGetTime();
        if (current_time <= bd->Time)
            current_time = bd->Time + 0.00001f;
        io.DeltaTime = bd->Time > 0.0 ? (float)(current_time - bd->Time) : (float)(1.0f / 60.0f);
        bd->Time = current_time;
    }

    inline void GLFW_VK_ImGuiContext::newFrame_UpdateMonitors()
    {
        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();

        int monitors_count = 0;
        GLFWmonitor** glfw_monitors = glfwGetMonitors(&monitors_count);
        if (monitors_count == 0)
            return;

        platform_io.Monitors.resize(0);
        for (int n = 0; n < monitors_count; n++)
        {
            ImGuiPlatformMonitor monitor;
            int x, y;
            glfwGetMonitorPos(glfw_monitors[n], &x, &y);
            const GLFWvidmode* vid_mode = glfwGetVideoMode(glfw_monitors[n]);
            if (vid_mode == nullptr)
                continue;
            monitor.MainPos = monitor.WorkPos = ImVec2((float)x, (float)y);
            monitor.MainSize = monitor.WorkSize = ImVec2((float)vid_mode->width, (float)vid_mode->height);

#if GLFW_HAS_MONITOR_WORK_AREA
            int w, h;
            glfwGetMonitorWorkarea(glfw_monitors[n], &x, &y, &w, &h);
            if (w > 0 && h > 0)
            {
                monitor.WorkPos = ImVec2((float)x, (float)y);
                monitor.WorkSize = ImVec2((float)w, (float)h);
            }
#endif

#if GLFW_HAS_PER_MONITOR_DPI
            float x_scale, y_scale;
            glfwGetMonitorContentScale(glfw_monitors[n], &x_scale, &y_scale);
            if (x_scale == 0.0f)
                continue;
            monitor.DpiScale = x_scale;
#endif

            monitor.PlatformHandle = (void*)glfw_monitors[n];
            platform_io.Monitors.push_back(monitor);
        }
    }
    static inline float Saturate(float v) { return v < 0.0f ? 0.0f : v  > 1.0f ? 1.0f : v; }

   inline void GLFW_VK_ImGuiContext::newFrame_updateMouseCursor()
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
        if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(bd->Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
            return;

        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
        for (int n = 0; n < platform_io.Viewports.Size; n++)
        {
            GLFWwindow* window = (GLFWwindow*)platform_io.Viewports[n]->PlatformHandle;
            if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
            {
                // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
                m_wnd->sendUiTask([](void* data) {
                    auto params = static_cast<std::tuple<GLFWwindow*, int, int>*>(data);
                    glfwSetInputMode(std::get<0>(*params), std::get<1>(*params), std::get<2>(*params));
                    delete params;
                }, new std::tuple<GLFWwindow*, int, int>(
                    window,
                    GLFW_CURSOR,
                    GLFW_CURSOR_HIDDEN
                ),false);
            }
            else
            {
                // Show OS mouse cursor
                // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
                m_wnd->sendUiTask([bd](void* data) {
                    auto params = static_cast<std::tuple<GLFWwindow*, ImGuiMouseCursor>*>(data);
                    GLFWwindow* window = std::get<0>(*params);
                    ImGuiMouseCursor cursor = std::get<1>(*params);
                    glfwSetCursor(window, bd->MouseCursors[cursor] ? bd->MouseCursors[cursor] : bd->MouseCursors[ImGuiMouseCursor_Arrow]);
                    delete params;
                }, new std::tuple<GLFWwindow*, ImGuiMouseCursor>(
                    window,
                    imgui_cursor
                ),false);

                m_wnd->sendUiTask([](void* data) {
                    auto params = static_cast<std::tuple<GLFWwindow*, int, int>*>(data);
                    glfwSetInputMode(std::get<0>(*params), std::get<1>(*params), std::get<2>(*params));
                    delete params;
                }, new std::tuple<GLFWwindow*, int, int>(
                    window,
                    GLFW_CURSOR,
                    GLFW_CURSOR_NORMAL
                ),false);
            }
        }
    }

   inline void GLFW_VK_ImGuiContext::newFrame_UpdateGamepads()
   {
    ImGuiIO& io = ImGui::GetIO();
    if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0) // FIXME: Technically feeding gamepad shouldn't depend on this now that they are regular inputs, but see #8075
        return;

    io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
#if GLFW_HAS_GAMEPAD_API && !defined(EMSCRIPTEN_USE_EMBEDDED_GLFW3)
    GLFWgamepadstate gamepad;
    if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad))
        return;
    #define MAP_BUTTON(KEY_NO, BUTTON_NO, _UNUSED)          do { io.AddKeyEvent(KEY_NO, gamepad.buttons[BUTTON_NO] != 0); } while (0)
    #define MAP_ANALOG(KEY_NO, AXIS_NO, _UNUSED, V0, V1)    do { float v = gamepad.axes[AXIS_NO]; v = (v - V0) / (V1 - V0); io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, Saturate(v)); } while (0)
#else
    int axes_count = 0, buttons_count = 0;
    const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
    const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
    if (axes_count == 0 || buttons_count == 0)
        return;
    #define MAP_BUTTON(KEY_NO, _UNUSED, BUTTON_NO)          do { io.AddKeyEvent(KEY_NO, (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS)); } while (0)
    #define MAP_ANALOG(KEY_NO, _UNUSED, AXIS_NO, V0, V1)    do { float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; v = (v - V0) / (V1 - V0); io.AddKeyAnalogEvent(KEY_NO, v > 0.10f, Saturate(v)); } while (0)
#endif
    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
    MAP_BUTTON(ImGuiKey_GamepadStart,       GLFW_GAMEPAD_BUTTON_START,          7);
    MAP_BUTTON(ImGuiKey_GamepadBack,        GLFW_GAMEPAD_BUTTON_BACK,           6);
    MAP_BUTTON(ImGuiKey_GamepadFaceLeft,    GLFW_GAMEPAD_BUTTON_X,              2);     // Xbox X, PS Square
    MAP_BUTTON(ImGuiKey_GamepadFaceRight,   GLFW_GAMEPAD_BUTTON_B,              1);     // Xbox B, PS Circle
    MAP_BUTTON(ImGuiKey_GamepadFaceUp,      GLFW_GAMEPAD_BUTTON_Y,              3);     // Xbox Y, PS Triangle
    MAP_BUTTON(ImGuiKey_GamepadFaceDown,    GLFW_GAMEPAD_BUTTON_A,              0);     // Xbox A, PS Cross
    MAP_BUTTON(ImGuiKey_GamepadDpadLeft,    GLFW_GAMEPAD_BUTTON_DPAD_LEFT,      13);
    MAP_BUTTON(ImGuiKey_GamepadDpadRight,   GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,     11);
    MAP_BUTTON(ImGuiKey_GamepadDpadUp,      GLFW_GAMEPAD_BUTTON_DPAD_UP,        10);
    MAP_BUTTON(ImGuiKey_GamepadDpadDown,    GLFW_GAMEPAD_BUTTON_DPAD_DOWN,      12);
    MAP_BUTTON(ImGuiKey_GamepadL1,          GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,    4);
    MAP_BUTTON(ImGuiKey_GamepadR1,          GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,   5);
    MAP_ANALOG(ImGuiKey_GamepadL2,          GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,     4,      -0.75f,  +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadR2,          GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,    5,      -0.75f,  +1.0f);
    MAP_BUTTON(ImGuiKey_GamepadL3,          GLFW_GAMEPAD_BUTTON_LEFT_THUMB,     8);
    MAP_BUTTON(ImGuiKey_GamepadR3,          GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,    9);
    MAP_ANALOG(ImGuiKey_GamepadLStickLeft,  GLFW_GAMEPAD_AXIS_LEFT_X,           0,      -0.25f,  -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadLStickRight, GLFW_GAMEPAD_AXIS_LEFT_X,           0,      +0.25f,  +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadLStickUp,    GLFW_GAMEPAD_AXIS_LEFT_Y,           1,      -0.25f,  -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadLStickDown,  GLFW_GAMEPAD_AXIS_LEFT_Y,           1,      +0.25f,  +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickLeft,  GLFW_GAMEPAD_AXIS_RIGHT_X,          2,      -0.25f,  -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickRight, GLFW_GAMEPAD_AXIS_RIGHT_X,          2,      +0.25f,  +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickUp,    GLFW_GAMEPAD_AXIS_RIGHT_Y,          3,      -0.25f,  -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickDown,  GLFW_GAMEPAD_AXIS_RIGHT_Y,          3,      +0.25f,  +1.0f);
    #undef MAP_BUTTON
    #undef MAP_ANALOG
   }

   inline void GLFW_VK_ImGuiContext::newFrame_updateInput()
   {
        ImGui_ImplGlfw_Data* bd = ImGui_ImplGlfw_GetBackendData();
        bd->MouseIgnoreButtonUp = false;
        newFrame_UpdateMouseData();
        newFrame_updateMouseCursor();
        newFrame_UpdateGamepads();
   }
}
#endif //VK_GLFW_IMGUICONTEXT_H
