//
// Created by Administrator on 2025/3/30.
//
#include "./Image.h"
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

        void create() override;
        void create(RHI::Image* image);
        void as(ImageUsageFlags usage) override;
        void bind(Context* ctx) override;

        Image* getImage() const override { return const_cast<SingleBufferImage*>(this); }
        std::vector<Image*> getTargetImages() override;

        RHI::RenderTargetView* currentTargetView() override;
    protected:
        RHI::Image* m_image;
        RHI::RenderTargetView* m_rtv;
    };
}


#endif //SINGLEBUFFERIMAGE_H
