#include "../ALL_Headers.h"
#include "Android_Window.h"


namespace FCT{
    void Android_Window::bind(FCT::Context *ctx) {

    }

    bool Android_Window::isRunning() const {
        return m_running;
    }

    void Android_Window::swapBuffers() {

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
        m_created = true;
        m_running = true;
    }

    void Android_Window::destroy() {
        m_running = false;
    }

    bool Android_Window::isCreated() const {
        return m_created;
    }

    void Android_Window::createSurface() {
        m_hasSurface = true;
    }

    void Android_Window::destroySurface() {
        m_hasSurface = false;
    }
}