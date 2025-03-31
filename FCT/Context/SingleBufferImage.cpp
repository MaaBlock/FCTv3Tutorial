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
    }

    SingleBufferImage::~SingleBufferImage() {
        FCT_SAFE_RELEASE(m_image);
        FCT_SAFE_RELEASE(m_rtv);
    }

    void SingleBufferImage::create() {
        FCT_SAFE_RELEASE(m_image);
        /*m_image = m_ctx->createImage(); // 假设 Context 有 createImage 方法
        m_image->create(m_width, m_height, m_format, m_samples);
        delete m_behavior;*/
        //m_behavior = new AfterCreateImageBehavior(this);
    }

    void SingleBufferImage::create(RHI::Image* image) {
        FCT_SAFE_RELEASE(m_image);
        m_image = image;
        m_image->addRef();
        delete m_behavior;
        m_behavior = new SingleBufferAfterCreateImageBehavior(this);
    }

    void SingleBufferImage::as(ImageUsageFlags usage) {
        if (usage & ImageUsage::RenderTarget && !m_rtv) {
            m_rtv = m_ctx->createRenderTargetView();
            m_rtv->image(m_image);
            m_rtv->create();
        }
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
}
