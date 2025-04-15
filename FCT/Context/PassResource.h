//
// Created by Administrator on 2025/4/14.
//

#ifndef FCT_PASSRESOURCE_H
#define FCT_PASSRESOURCE_H
#include "../RHI/ConstBuffer.h"
#include "../UI/Window.h"
namespace FCT
{
    class PassResource
    {
    public:
        PassResource()
        {
            m_pipeline = nullptr;
        }
        //todo:整个系统变更成使用IRenderTarget
        void bind(Window* wnd)
        {
            m_wnd = wnd;
        }
        virtual void bind(RHI::CommandBuffer* cmdBuf) = 0;
        virtual void addConstBuffer(RHI::ConstBuffer* buffer) = 0;
        virtual void create() = 0;
        virtual void update() = 0;
        void pipeline(RHI::Pipeline* pipeline)
        {
            m_pipeline = pipeline;
        }
    protected:
        std::vector<RHI::ConstBuffer*> m_constBuffers;
        RHI::Pipeline* m_pipeline;
        Window* m_wnd;
        bool m_dirty = true;
    };
}
#endif //PASSRESOURCE_H
