//
// Created by Administrator on 2025/3/27.
//

#include "../headers.h"
#include "./Image.h"
namespace FCT
{
    BeforeCreateImageBehavior::BeforeCreateImageBehavior(Image* image)
    {
        m_image = image;
    }

    Format BeforeCreateImageBehavior::format() const
    {
        return m_image->m_format;
    }

    Samples BeforeCreateImageBehavior::samples() const
    {
        return m_image->m_samples;
    }

    AfterCreateImageBehavior::AfterCreateImageBehavior(Image* image)
    {
        m_image = image;
    }

    Format AfterCreateImageBehavior::format() const
    {
        return m_image->m_image->format();
    }

    Samples AfterCreateImageBehavior::samples() const
    {
        return m_image->m_image->samples();
    }

    Image::Image(Context* ctx)
    {
        m_renderTargetType = RenderTargetType::ImageTarget;
        m_ctx = ctx;
        m_image = nullptr;
        m_rtv = nullptr;
        m_behavior = new BeforeCreateImageBehavior(this);
    }

    Image::~Image()
    {
        FCT_SAFE_RELEASE(m_image);
    }

    void Image::create(RHI::Image* image)
    {
        m_image = image;
        m_image->addRef();
        if (m_behavior)
        {
            delete m_behavior;
            m_behavior = nullptr;
        }
        m_behavior = new AfterCreateImageBehavior(this);
    }

    void Image::as(ImageUsageFlags usage)
    {
        if (usage & ImageUsage::RenderTarget && !m_rtv)
        {
            m_rtv = m_ctx->createRenderTargetView();
            m_rtv->image(m_image);
            m_rtv->create();
        }
    }
}
