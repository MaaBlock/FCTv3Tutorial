//
// Created by Administrator on 2025/2/20.
//
#include "Window.h"
#ifndef ANDROID_WINDOW_H
#define ANDROID_WINDOW_H
namespace FCT {
    class Android_Window : public Window {
    public:
        void bind(FCT::Context *ctx) override;

        bool isRunning() const override;

        void swapBuffers() override;

        int getWidth() override;

        int getHeight() override;

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

        void createSurface();

        void destroySurface();
        void create(float w,float h,const char* title);

        void viewport(Vec2 lt, Vec2 rb) override;

    private:
        bool m_hasSurface;
        bool m_running;
        bool m_created;
        const char *m_title;
        Vec2 m_lt;
        Vec2 m_rb;
    };
}

#endif //ANDROID_WINDOW_H
