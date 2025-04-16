//
// Created by Administrator on 2025/4/14.
//

#ifndef FCT_PASSRESOURCE_H
#define FCT_PASSRESOURCE_H
#include "../RHI/ConstBuffer.h"
#include "../UI/Window.h"
namespace FCT
{
    namespace RHI
    {
        class Sampler;
    }

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
        virtual void addTexture(Image* texture,TextureElement element) = 0;
        virtual void addSampler(RHI::Sampler* sampler,SamplerElement element) = 0;
        virtual void create() = 0;
        virtual void update() = 0;
        void pipeline(RHI::Pipeline* pipeline)
        {
            m_pipeline = pipeline;
        }
    protected:
        std::vector<RHI::ConstBuffer*> m_constBuffers;
        std::vector<std::pair<RHI::Sampler*,SamplerElement>> m_samplers;
        std::vector<std::pair<Image*, TextureElement>> m_textures;
        RHI::Pipeline* m_pipeline;
        Window* m_wnd;
        bool m_dirty = true;
    };
}
#endif //PASSRESOURCE_H
