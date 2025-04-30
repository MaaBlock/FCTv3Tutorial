//
// Created by Administrator on 2025/3/30.
//
#include "./Image.h"
#include "../RHI/TextureView.h"
#ifndef SINGLEBUFFERIMAGE_H
#define SINGLEBUFFERIMAGE_H
namespace FCT
{

    class SingleBufferAfterCreateImageBehavior : public ImageBehavior
    {
    public:
        SingleBufferAfterCreateImageBehavior(Image* image);
        Format format() const;
        Samples samples() const;
    };
    class SingleBufferImage : public Image {
    public:
        friend class SingleBufferAfterCreateImageBehavior;
        SingleBufferImage(Context* ctx);
        ~SingleBufferImage() override;
        void initData(void* data,int size)
        {
            m_initData.data = data;
            m_initData.size = size;
        }
        void create() override;
        void create(RHI::Image* image);
        void as(ImageUsageFlags usage) override;
        void bind(Context* ctx) override;
        void resize(uint32_t width, uint32_t height) override;
        Image* getImage() const override { return const_cast<SingleBufferImage*>(this); }
        std::vector<Image*> getTargetImages() override;

        RHI::RenderTargetView* currentTargetView() override;
        RHI::TextureView* currentTextureView() override;
        RHI::DepthStencilView* currentDepthStencilView() override;
        RHI::Image* currentImage() override;
    protected:
        RHI::Image* m_image;
        RHI::RenderTargetView* m_rtv;
        RHI::TextureView* m_srv;
        ImageInitData m_initData;
        ImageUsageFlags m_usage;
    };
}


#endif //SINGLEBUFFERIMAGE_H
