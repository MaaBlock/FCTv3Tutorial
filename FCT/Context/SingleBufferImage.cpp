//
// Created by Administrator on 2025/3/30.
//

#include "../headers.h"

namespace FCT
{
    SingleBufferAfterCreateImageBehavior::SingleBufferAfterCreateImageBehavior(Image* image) {
        m_image = image;
    }

    Format SingleBufferAfterCreateImageBehavior::format() const {
        return static_cast<SingleBufferImage*>(m_image)->m_image->format();
    }

    Samples SingleBufferAfterCreateImageBehavior::samples() const {
        return static_cast<SingleBufferImage*>(m_image)->m_image->samples();
    }

    SingleBufferImage::SingleBufferImage(Context* ctx) : Image(ctx), m_image(nullptr), m_rtv(nullptr) {
        m_behavior = new BeforeCreateImageBehavior(this);
        m_srv = nullptr;
    }

    SingleBufferImage::~SingleBufferImage() {
        FCT_SAFE_RELEASE(m_image);
        FCT_SAFE_RELEASE(m_rtv);
    }

    void SingleBufferImage::create() {
        FCT_SAFE_RELEASE(m_image);
        m_image = m_ctx->newRhiImage();
        m_image->width(m_width);
        m_image->height(m_height);
        m_image->format(m_format);
        m_image->samples(m_samples);
        m_image->initData(m_initData);
        m_image->usage(m_usage);
        m_image->create();
        if (m_usage & ImageUsage::Texture && !m_srv)
        {
            m_srv = m_ctx->createTextureView();
            m_srv->image(m_image);
            m_srv->create();
        }
        delete m_behavior;
        m_behavior = new SingleBufferAfterCreateImageBehavior(this);
    }

    void SingleBufferImage::create(RHI::Image* image) {
        FCT_SAFE_RELEASE(m_image);
        m_image = image;
        m_image->addRef();
        m_width = image->width();
        m_height = image->height();
        delete m_behavior;
        m_behavior = new SingleBufferAfterCreateImageBehavior(this);
    }

    void SingleBufferImage::as(ImageUsageFlags usage) {
        if (usage & ImageUsage::RenderTarget && !m_rtv) {
            m_rtv = m_ctx->createRenderTargetView();
            m_rtv->image(m_image);
            m_rtv->create();
        }
        m_usage |= usage;
    }

    void SingleBufferImage::bind(Context* ctx) {

    }

    std::vector<Image*> SingleBufferImage::getTargetImages() {
        if (m_rtv) {
            return {this};
        }
        return {};
    }

    RHI::RenderTargetView* SingleBufferImage::currentTargetView()
    {
        return m_rtv;
    }

    RHI::TextureView* SingleBufferImage::currentTextureView()
    {
        return m_srv;
    }

    RHI::DepthStencilView* SingleBufferImage::currentDepthStencilView()
    {
        return nullptr;
    }
}
