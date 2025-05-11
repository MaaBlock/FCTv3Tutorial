#pragma once
#include "./EventHandler.h"
#include <vector>
#include <functional>
#include <unordered_map>

namespace FCT {
    using ResizeCallBack = std::function<void(Window* wnd, int width, int height)>;
    class CallBackEventHandler : public EventHandler {
    public:
        using LButtonDownCallBack = std::function<void(Window* wnd, int x, int y)>;
        using LButtonUpCallBack = std::function<void(Window* wnd, int x, int y)>;
        using RButtonDownCallBack = std::function<void(Window* wnd, int x, int y)>;
        using RButtonUpCallBack = std::function<void(Window* wnd, int x, int y)>;
        using MouseMoveCallBack = std::function<void(Window* wnd, int x, int y)>;
        using MouseWheelCallBack = std::function<void(Window* wnd, int delta)>;
        using KeyDownCallBack = std::function<void(Window* wnd, int key)>;
        using KeyUpCallBack = std::function<void(Window* wnd, int key)>;
        using CallbackId = size_t;

        void onResize(Window* wnd, int width, int height) override {
            for (const auto& pair : m_resizeCallbacks) {
                pair.second(wnd, width, height);
            }
        }
        void onMouseMove(Window* wnd, int x, int y) override {
            for (const auto& pair : m_mouseMoveCallbacks) {
                pair.second(wnd, x, y);
            }
        }
        void onMouseWheel(Window* wnd, int delta) override {
            for (const auto& pair : m_mouseWheelCallbacks) {
                pair.second(wnd, delta);
            }
        }
        void onKeyDown(Window* wnd, int key) override {
            for (const auto& pair : m_keyDownCallbacks) {
                pair.second(wnd, key);
            }
        }
        void onKeyUp(Window* wnd, int key) override {
            for (const auto& pair : m_keyUpCallbacks) {
                pair.second(wnd, key);
            }
        }
        void onLButtonDown(Window* wnd, int x, int y) override {
            for (const auto& pair : m_lButtonDownCallbacks) {
                pair.second(wnd, x, y);
            }
        }
        void onLButtonUp(Window* wnd, int x, int y) override {
            for (const auto& pair : m_lButtonUpCallbacks) {
                pair.second(wnd, x, y);
            }
        }
        void onRButtonDown(Window* wnd, int x, int y) override {
            for (const auto& pair : m_rButtonDownCallbacks) {
                pair.second(wnd, x, y);
            }
        }
        void onRButtonUp(Window* wnd, int x, int y) override {
            for (const auto& pair : m_rButtonUpCallbacks) {
                pair.second(wnd, x, y);
            }
        }

        CallbackId addResizeCallback(const ResizeCallBack& cb) {
            CallbackId id = m_nextId++;
            m_resizeCallbacks[id] = cb;
            return id;
        }

        CallbackId addLButtonDownCallback(const LButtonDownCallBack& cb) {
            CallbackId id = m_nextId++;
            m_lButtonDownCallbacks[id] = cb;
            return id;
        }
        CallbackId addLButtonUpCallback(const LButtonUpCallBack& cb) {
            CallbackId id = m_nextId++;
            m_lButtonUpCallbacks[id] = cb;
            return id;
        }
        CallbackId addRButtonDownCallback(const RButtonDownCallBack& cb) {
            CallbackId id = m_nextId++;
            m_rButtonDownCallbacks[id] = cb;
            return id;
        }
        CallbackId addRButtonUpCallback(const RButtonUpCallBack& cb) {
            CallbackId id = m_nextId++;
            m_rButtonUpCallbacks[id] = cb;
            return id;
        }
        CallbackId addMouseMoveCallback(const MouseMoveCallBack& cb) {
            CallbackId id = m_nextId++;
            m_mouseMoveCallbacks[id] = cb;
            return id;
        }
        CallbackId addMouseWheelCallback(const MouseWheelCallBack& cb) {
            CallbackId id = m_nextId++;
            m_mouseWheelCallbacks[id] = cb;
            return id;
        }
        CallbackId addKeyDownCallback(const KeyDownCallBack& cb) {
            CallbackId id = m_nextId++;
            m_keyDownCallbacks[id] = cb;
            return id;
        }
        CallbackId addKeyUpCallback(const KeyUpCallBack& cb) {
            CallbackId id = m_nextId++;
            m_keyUpCallbacks[id] = cb;
            return id;
        }
        void removeResizeCallback(CallbackId id) {
            m_resizeCallbacks.erase(id);
        }

        void removeLButtonDownCallback(CallbackId id) {
            m_lButtonDownCallbacks.erase(id);
        }
        void removeLButtonUpCallback(CallbackId id) {
            m_lButtonUpCallbacks.erase(id);
        }
        void removeRButtonDownCallback(CallbackId id) {
            m_rButtonDownCallbacks.erase(id);
        }
        void removeRButtonUpCallback(CallbackId id) {
            m_rButtonUpCallbacks.erase(id);
        }
        void removeMouseMoveCallback(CallbackId id) {
            m_mouseMoveCallbacks.erase(id);
        }
        void removeMouseWheelCallback(CallbackId id) {
            m_mouseWheelCallbacks.erase(id);
        }
        void removeKeyDownCallback(CallbackId id) {
            m_keyDownCallbacks.erase(id);
        }
        void removeKeyUpCallback(CallbackId id) {
            m_keyUpCallbacks.erase(id);
        }

        void clearResizeCallbacks() {
            m_resizeCallbacks.clear();
        }
        void clearLButtonDownCallbacks() {
            m_lButtonDownCallbacks.clear();
        }
        void clearLButtonUpCallbacks() {
            m_lButtonUpCallbacks.clear();
        }
        void clearRButtonDownCallbacks() {
            m_rButtonDownCallbacks.clear();
        }
        void clearRButtonUpCallbacks() {
            m_rButtonUpCallbacks.clear();
        }
        void clearMouseMoveCallbacks() {
            m_mouseMoveCallbacks.clear();
        }
        void clearMouseWheelCallbacks() {
            m_mouseWheelCallbacks.clear();
        }
        void clearKeyDownCallbacks() {
            m_keyDownCallbacks.clear();
        }
        void clearKeyUpCallbacks() {
            m_keyUpCallbacks.clear();
        }
        void invokeResizeCallbacks(Window* wnd, int width, int height) {
            onResize(wnd, width, height);
        }

    private:
        std::unordered_map<CallbackId, ResizeCallBack> m_resizeCallbacks;
        std::unordered_map<CallbackId, LButtonDownCallBack> m_lButtonDownCallbacks;
        std::unordered_map<CallbackId, LButtonUpCallBack> m_lButtonUpCallbacks;
        std::unordered_map<CallbackId, RButtonDownCallBack> m_rButtonDownCallbacks;
        std::unordered_map<CallbackId, RButtonUpCallBack> m_rButtonUpCallbacks;
        std::unordered_map<CallbackId, MouseMoveCallBack> m_mouseMoveCallbacks;
        std::unordered_map<CallbackId, MouseWheelCallBack> m_mouseWheelCallbacks;
        std::unordered_map<CallbackId, KeyDownCallBack> m_keyDownCallbacks;
        std::unordered_map<CallbackId, KeyUpCallBack> m_keyUpCallbacks;
        CallbackId m_nextId = 0;
    };
}