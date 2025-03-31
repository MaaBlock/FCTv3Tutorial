//
// Created by Administrator on 2025/3/30.
//
#include "./Image.h"
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
        void create() override;
        void create(std::vector<RHI::Image*> images);
        void as(ImageUsageFlags usage) override;
        void bind(Context* ctx) override;
        Image* getImage() const override { return const_cast<MutilBufferImage*>(this); }
        std::vector<Image*> getTargetImages() override;
        RHI::RenderTargetView* currentTargetView() override;
    private:
        size_t m_currentIndex = 0;
        std::vector<RHI::Image*> m_images;
        std::vector<RHI::RenderTargetView*> m_rtvs;
    };

}


#endif //MUTILBUFFERIMAGE_H
