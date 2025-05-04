//
// Created by Administrator on 2025/3/30.
//

#include "../FCTAPI.h"
namespace FCT
{
    MutilBufferAffterCreateImageBehavior::MutilBufferAffterCreateImageBehavior(MutilBufferImage* image)
    {
        m_image = image;
    }

    Format MutilBufferAffterCreateImageBehavior::format() const
    {
        return static_cast<MutilBufferImage*>(m_image)->m_images[0]->format();
    }

    Samples MutilBufferAffterCreateImageBehavior::samples() const
    {
        return static_cast<MutilBufferImage*>(m_image)->m_images[0]->samples();
    }

    MutilBufferImage::MutilBufferImage(Context* ctx) : Image(ctx)
    {
        m_behavior = new BeforeCreateImageBehavior(this);
    }

    void MutilBufferImage::create()
    {
        for (uint32_t i = 0; i < m_imageCount; ++i)
        {
            auto image = m_ctx->newRhiImage();
            image->width(m_width);
            image->height(m_height);
            image->format(m_format);
            image->samples(m_samples);
            image->usage(m_usage);
            image->create();
            m_images.push_back(image);
        }

        delete m_behavior;
        m_behavior = new MutilBufferAffterCreateImageBehavior(this);

        as(m_usage);

        if (m_currentIndex >= m_images.size() && !m_images.empty())
        {
            m_currentIndex = 0;
        }
    }
    void MutilBufferImage::resize(uint32_t width, uint32_t height)
    {
        if (m_width == width && m_height == height)
        {
            return;
        }

        Format currentFormat = m_format;
        Samples currentSamples = m_samples;
        ImageUsageFlags currentUsage = m_usage;
        uint32_t currentImageCount = m_imageCount;

        for (auto img : m_images)
        {
            FCT_SAFE_RELEASE(img);
        }
        m_images.clear();

        for (auto rtv : m_rtvs)
        {
            rtv->release();
        }
        m_rtvs.clear();

        for (auto dsv : m_dsvs)
        {
            dsv->release();
        }
        m_dsvs.clear();
        for (auto tv : m_tvs)
        {
            tv->release();
        }
        m_tvs.clear();

        m_width = width;
        m_height = height;

        for (uint32_t i = 0; i < currentImageCount; ++i)
        {
            auto image = m_ctx->newRhiImage();
            image->width(m_width);
            image->height(m_height);
            image->format(currentFormat);
            image->samples(currentSamples);
            image->usage(currentUsage);
            image->create();
            m_images.push_back(image);
        }

        as(currentUsage);

        if (m_currentIndex >= m_images.size() && !m_images.empty())
        {
            m_currentIndex = 0;
        }

    }
    void MutilBufferImage::create(std::vector<RHI::Image*> images)
    {
        for (auto img : m_images)
        {
            FCT_SAFE_RELEASE(img);
        }
        for (auto rtv : m_rtvs)
        {
            rtv->release();
        }
        for (auto dsv : m_dsvs)
        {
            dsv->release();
        }
        for (auto tv : m_tvs)
        {
            tv->release();
        }
        m_rtvs.clear();
        m_dsvs.clear();
        m_tvs.clear();
        m_images = images;
        for (auto img : m_images)
        {
            FCT_SAFE_ADDREF(img);
        }
        m_width = images[0]->width();
        m_height = images[0]->height();
        delete m_behavior;
        m_behavior = new MutilBufferAffterCreateImageBehavior(this);
        as(m_usage);
    }

    void MutilBufferImage::as(ImageUsageFlags usage)
    {
        m_usage |= usage;
        if (usage & ImageUsage::RenderTarget && m_rtvs.empty())
        {
            for (auto img : m_images)
            {
                auto rtv = m_ctx->createRenderTargetView();
                rtv->image(img);
                rtv->create();
                m_rtvs.push_back(rtv);
            }
        }
        if (usage & ImageUsage::DepthStencil && m_dsvs.empty())
        {
            for (auto img : m_images)
            {
                auto dsv = m_ctx->createDepthStencilView();
                dsv->image(img);
                while (!dsv->create());
                m_dsvs.push_back(dsv);
            }
        }
        if (usage & ImageUsage::Texture && m_tvs.empty())
        {
            for (auto img : m_images)
            {
                auto tv = m_ctx->createTextureView();
                tv->image(img);
                tv->create();
                m_tvs.push_back(tv);
            }
        }
    }

    void MutilBufferImage::bind(Context* ctx)
    {
        Image::bind(ctx);
    }

    std::vector<Image*> MutilBufferImage::getTargetImages()
    {
        if (m_rtvs.size()) {
            return {this};
        }
        return {};
    }

    RHI::RenderTargetView* MutilBufferImage::currentTargetView()
    {
        return m_rtvs[m_currentIndex];
    }

    RHI::TextureView* MutilBufferImage::currentTextureView()
    {
        fout << "has gotten " << m_currentIndex << " resource" << std::endl;
        return m_tvs[m_currentIndex];
    }

    RHI::DepthStencilView* MutilBufferImage::currentDepthStencilView()
    {
        return m_dsvs[m_currentIndex];
    }

    RHI::Image* MutilBufferImage::currentImage()
    {
        return m_images[m_currentIndex];
    }

    UpdateResult* MutilBufferImage::updateToCurrent(void* data, size_t size)
    {
        UpdateResult* res = new UpdateResult();
        res->fence = m_ctx->createFence();
        m_images[m_ctx->currentSubmitFrameIndex()]->updateData(data,size,res->fence,&res->cleanUpCallback);
        res->currentFrame = m_ctx->currentFrameIndex();

        return res;
    }
}
