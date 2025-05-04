//
// Created by Administrator on 2025/3/16.
//
#include "../ThirdParty.h"
#include "./Job.h"
#include "../RHI/Pass.h"
#include "./PassResource.h"
#include "../RHI/CommandBuffer.h"
#ifndef FCT_PASS_H
#define FCT_PASS_H
namespace FCT {
    namespace RHI
    {
        class RasterizationPipeline;
    }
    class Context;

    using RenderGraphPassResourceAddCallback = std::function<void(PassResource*)>;
    class Pass : public RefCount
    {
    protected:
        std::map<uint32_t,Image*> m_targets;
        Context* m_ctx;
        Image* m_depthStencil;
        PassClearValue m_clearValue;
        RHI::Pass* m_pass;
        RenderGraphPassResourceAddCallback m_addResourceCallback;
    public:
        Pass()
        {
            m_addResourceCallback = [](PassResource* resource)
            {

            };
        }
        void setPassResourceAddCallback(const RenderGraphPassResourceAddCallback& callback)
        {
            m_addResourceCallback = callback;
        }
        RHI::Pass* rhiPass() const
        {
            return m_pass;
        }
        void setRhiPass(RHI::Pass* pass)
        {
            m_pass = pass;
        }
        virtual ~Pass() = default;

        PassClearValue getClearValue() const
        {
            return m_clearValue;
        }
        Pass& enableClear(ClearTypes type,Vec4 color,float depth = 1.0f, uint8_t stencil = 0)
        {
            m_clearValue.types = type;
            m_clearValue.color = color;
            m_clearValue.depth = depth;
            m_clearValue.stencil = stencil;
            return *this;
        }
        virtual void submit(RHI::CommandBuffer* cmdBuf) = 0;
        virtual void swapJobQueue() = 0;
        void setTarget(uint32_t index, Image* target)
        {
            m_targets[index] = target;
        }
        void setDepthStencil(Image* depthStencil)
        {
            m_depthStencil = depthStencil;
        }
        void setTarget(uint32_t index,Window* wnd)
        {
            setTarget(index,wnd->getCurrentTarget()->getImage());
            setDepthStencil(wnd->getCurrentTarget()->depthStencilBuffer());
        }
        virtual void submit(Job* job) = 0;
    };

}
#endif //FCT_PASS_H
