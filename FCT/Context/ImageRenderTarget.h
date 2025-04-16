// Created by Administrator on 2025/3/27.
//
//
#include "../MutilThreadBase/RefCount.h"
#include "../RHI/Image.h"
#include "./IRenderTarget.h"
#ifndef IMAGERENDERTARGET_H
#include "../RHI/RenderTargetView.h"
#define IMAGERENDERTARGET_H

namespace FCT
{
    class ImageRenderTarget : public IRenderTarget,public RefCount{
    public:
        RenderTargetType getType() const override;
        ImageRenderTarget(Context* ctx);
        ~ImageRenderTarget();
        int width() const { return m_width; }
        int height() const { return m_height; }
        void witdh(int width) { m_width = width; }
        int height(int height) { m_height = height; }
        void bindTarget(RHI::Image* image);
        void bindTarget(Image* image);
        void setDepthStencilBuffer(Image* image);
        Image* targetImage() const { return m_target; }
        Image* depthStencilBuffer() const { return m_depthStencilBuffer; }
/*
        void bindRenderTarget(uint32_t index, Image* image);
        void bindRenderTarget(uint32_t index, RHI::Image* image);*/
        /*void addRenderTarget(Image* image);
        void addRenderTarget(RHI::Image* image);*/
        /*void addRenderTargets(std::vector<RHI::Image*> images)
        {
            for (auto& image : images)
            {
                addRenderTarget(image);
            }
        }
        void addRenderTarget(std::vector<RHI::Image*> images);*/
        void bind(Context* ctx){};
        //std::vector<Image*> getTargetImages() override;
        Image* getImage() const override
        {
            return nullptr;
        }
        void renderTargetType(RenderTargetType type) { m_renderTargetType = type; }
        //std::map<uint32_t, RHI::Image*> getTarget override;
    private:
        RenderTargetType m_renderTargetType;
        int m_width;
        int m_height;
        Context* m_ctx;
        Image* m_target;
        Image* m_depthStencilBuffer;
        //std::map<uint32_t,Image*> m_renderTargetImages;
        //std::vector<Image*> m_renderTargetImages;
    };
}


#endif //IMAGERENDERTARGET_H
