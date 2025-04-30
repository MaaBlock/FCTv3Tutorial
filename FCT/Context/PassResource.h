//
// Created by Administrator on 2025/4/14.
//

#ifndef FCT_PASSRESOURCE_H
#define FCT_PASSRESOURCE_H
#include "../RHI/ConstBuffer.h"
#include "../UI/Window.h"
namespace FCT
{
    class Sampler;
    class SamplerElement;
    class TextureElement;

    namespace RHI
    {
        class Pipeline;
        class CommandBuffer;
    }

    class PassResource
    {
    public:
        PassResource()
        {

        }
        //todo:整个系统变更成使用IRenderTarget
        void bind(Window* wnd)
        {
            m_wnd = wnd;
        }
        virtual void bind(RHI::CommandBuffer* cmdBuf,RHI::Pipeline* pipeline) = 0;
        virtual void addConstBuffer(RHI::ConstBuffer* buffer) = 0;
        virtual void addTexture(Image* texture,TextureElement element) = 0;
        virtual void addSampler(Sampler* sampler,SamplerElement element) = 0;
        virtual void create() = 0;
        virtual void update() = 0;
    protected:
        std::vector<RHI::ConstBuffer*> m_constBuffers;
        std::vector<std::pair<Sampler*,SamplerElement>> m_samplers;
        std::vector<std::pair<Image*, TextureElement>> m_textures;
        Window* m_wnd;
        bool m_dirty = true;
    };
}
#endif //PASSRESOURCE_H
