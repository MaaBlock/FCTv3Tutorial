//
// Created by Administrator on 2025/3/27.
//

#include "../FCTAPI.h"
namespace FCT
{
    RenderTargetType ImageRenderTarget::getType() const
    {
        return m_renderTargetType;
    }

    ImageRenderTarget::ImageRenderTarget(Context* ctx)
    {
        m_ctx = ctx;
        m_renderTargetType = RenderTargetType::ImageTarget;
    }

    ImageRenderTarget::~ImageRenderTarget()
    {
        for (auto target : m_renderTargetImages)
        {
            target->release();
        }
    }

    void ImageRenderTarget::addRenderTarget(RHI::Image* image)
    {
        auto renderTargetImage = new Image(m_ctx);
        renderTargetImage->renderTargetType(m_renderTargetType);
        renderTargetImage->create(image);
        if (m_renderTargetImages.empty())
        {
            m_width = image->width();
            m_height = image->height();
        }
#ifdef FCT_DEBUG
        else
        {
            if (m_width!= image->width() || m_height!= image->height())
            {
                ferr << "ImageRenderTarget::addRenderTarget: Image size not match!" << std::endl;
            }
        }
#endif
        m_renderTargetImages.push_back(renderTargetImage);
    }

    std::vector<Image*> ImageRenderTarget::getTargetImages()
    {
        return m_renderTargetImages;
    }
}
