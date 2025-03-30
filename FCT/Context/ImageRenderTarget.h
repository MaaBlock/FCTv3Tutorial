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
        void addRenderTarget(RHI::Image* image);
        void bind(Context* ctx){};
        std::vector<Image*> getTargetImages() override;
        Image* getImage() const override
        {
            return nullptr;
        }
        void renderTargetType(RenderTargetType type) { m_renderTargetType = type; }
    private:
        RenderTargetType m_renderTargetType;
        int m_width;
        int m_height;
        Context* m_ctx;
        std::vector<Image*> m_renderTargetImages;
    };
}


#endif //IMAGERENDERTARGET_H
