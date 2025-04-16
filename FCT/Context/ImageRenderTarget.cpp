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
        /*
        for (auto target : m_renderTargetImages)
        {
            target.second->release();
        }*/
        m_target->release();
    }

    void ImageRenderTarget::bindTarget(RHI::Image* image)
    {
        auto renderTargetImage = new SingleBufferImage(m_ctx);
        renderTargetImage->renderTargetType(m_renderTargetType);
        renderTargetImage->create(image);
        m_width = image->width();
        m_height = image->height();
    }

    void ImageRenderTarget::bindTarget(Image* image)
    {
        m_target = image;
        m_target->addRef();
    }

    void ImageRenderTarget::setDepthStencilBuffer(Image* image)
    {
        m_depthStencilBuffer = image;
        m_depthStencilBuffer->addRef();
    }

    /*
    void ImageRenderTarget::addRenderTarget(Image* image)
    {
        m_renderTargetImages.push_back(image);
    }
*/
    /*
    void ImageRenderTarget::addRenderTarget(RHI::Image* image)
    {
        auto renderTargetImage = new SingleBufferImage(m_ctx);
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
*/
    /*
    void ImageRenderTarget::addRenderTarget(std::vector<RHI::Image*> images)
    {

    }*/
/*
    void ImageRenderTarget::bindRenderTarget(uint32_t index, Image* image)
    {
        image->addRef();
        m_renderTargetImages[index] = image;
    }

    void ImageRenderTarget::bindRenderTarget(uint32_t index, RHI::Image* image)
    {
        auto renderTargetImage = new SingleBufferImage(m_ctx);
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
        m_renderTargetImages[index] = renderTargetImage;
    }*/
/*
    std::vector<Image*> ImageRenderTarget::getTargetImages()
    {
        return m_renderTargetImages;
    }*/
}
