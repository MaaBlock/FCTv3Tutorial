//
// Created by Administrator on 2025/5/10.
//

#ifndef INPUTSTATEEVENTHANNDLER_H
#define INPUTSTATEEVENTHANNDLER_H
#include "./Window.h"
#include "./EventHandler.h"
#include "./CallBackHandler.h"
namespace FCT
{
    using InputPosTransformCallback = std::function<bool(Vec2 pos,Vec2& ret)>;
    class InputStateEventHandler : public EventHandler
    {
    protected:
        InputPosTransformCallback m_posTransformCallback;
        ResizeCallBack m_resizeCallback;
        void onResize(Window* wnd, int width, int height) override
        {
            m_resizeCallback(wnd, width, height);
        }
        void onMouseMove(Window* wnd, int x, int y) override
        {

        }
        void onMouseWheel(Window* wnd, int delta) override
        {

        }
        struct InputState
        {
            int delta;
            Vec2 mouseMove;
            Vec2 mousePos;
            Vec2 mouseLastPos;
            char keyState[512];
            InputState() : delta(0), mouseMove(0, 0),
            mousePos(0, 0), mouseLastPos(0, 0),
            keyState{}
            {

            }
            void onSwap()
            {
                delta = 0;
            }
        } m_inputState[2];
        char m_keyState[512];
        InputState* m_logicInputState;
        InputState* m_uiInputState;
    public:
        InputStateEventHandler() : m_keyState{}
        {

        }
        void setPosTransformCallback(const InputPosTransformCallback& cb)
        {
            m_posTransformCallback = cb;
        }
        InputPosTransformCallback getPosTransformCallback() const
        {
            return m_posTransformCallback;
        }
        void setResizeCallback(const ResizeCallBack& cb)
        {
            m_resizeCallback = cb;
        }
        ResizeCallBack getResizeCallback() const
        {
            return m_resizeCallback;
        }
    };
}
#endif //INPUTSTATEEVENTHANNDLER_H
