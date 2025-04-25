//
// Created by Administrator on 2025/3/30.
//
#include "./Image.h"
#include "../RHI/DepthStencilView.h"
#ifndef MUTILBUFFERIMAGE_H
#define MUTILBUFFERIMAGE_H


namespace FCT
{
    class MutilBufferImage;
    class MutilBufferAffterCreateImageBehavior : public ImageBehavior
    {
    public:
        MutilBufferAffterCreateImageBehavior(MutilBufferImage* image);
        Format format() const override;
        Samples samples() const override;
    private:
    };
    class MutilBufferImage : public Image{
    public:
        friend class MutilBufferAffterCreateImageBehavior;
        MutilBufferImage(Context* ctx);
        void imageCount(size_t count) { m_imageCount = count; }
        void create() override;
        void resize(uint32_t width, uint32_t height);
        void create(std::vector<RHI::Image*> images);
        void as(ImageUsageFlags usage) override;
        void bind(Context* ctx) override;
        Image* getImage() const override { return const_cast<MutilBufferImage*>(this); }
        std::vector<Image*> getTargetImages() override;
        RHI::RenderTargetView* currentTargetView() override;
        RHI::TextureView* currentTextureView() override;
        RHI::DepthStencilView* currentDepthStencilView() override;
        RHI::Image* currentImage() override;
        void changeCurrentIndex(int index)
        {
            m_currentIndex = index % m_images.size();
        }
    private:
        size_t m_imageCount = 0;
        ImageUsageFlags m_usage;
        size_t m_currentIndex = 0;
        std::vector<RHI::Image*> m_images;
        std::vector<RHI::RenderTargetView*> m_rtvs;
        std::vector<RHI::DepthStencilView*> m_dsvs;
    };

}


#endif //MUTILBUFFERIMAGE_H
