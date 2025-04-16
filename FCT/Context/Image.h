//
// Created by Administrator on 2025/3/5.
//
#include "../MutilThreadBase/RefCount.h"
#include "../Base/Flags.h"
#include "./IRenderTarget.h"
#include "./Format.h"
#include "../RHI/DepthStencilView.h"
#include "../RHI/Image.h"
#include "../RHI/RenderTargetView.h"
#include "../RHI/TextureView.h"

#ifndef FCT_IMAGE_H
#define FCT_IMAGE_H

namespace FCT {
    class Context;
    /*
    struct ImageUsageFlags
    {
        ImageUsageFlags() : m_mask(0) {}
        ImageUsageFlags(ImageUsage bits) : m_mask(static_cast<unsigned int>(bits)) {}
        ImageUsageFlags(unsigned int flags) : m_mask(flags) {}

        operator unsigned int() const { return m_mask; }

        bool operator!() const { return !m_mask; }

        ImageUsageFlags operator&(ImageUsage bits) const
        {
            return ImageUsageFlags(m_mask & static_cast<unsigned int>(bits));
        }

        ImageUsageFlags& operator|=(ImageUsage bits)
        {
            m_mask |= static_cast<unsigned int>(bits);
            return *this;
        }

        unsigned int m_mask;
    }; */
    class Image;
    class ImageBehavior
    {
    public:
        virtual Format format() const = 0;
        virtual Samples samples() const = 0;
    protected:
        Image* m_image;
    };
    class BeforeCreateImageBehavior : public ImageBehavior
    {
    public:
        BeforeCreateImageBehavior(Image* image);
        Format format() const;
        Samples samples() const;
    private:

    };
#ifdef FCT_DEPRECATED

#endif // FCT_DEPRECATED
    class Image : public RefCount, public IRenderTarget {
    public:
        friend class BeforeCreateImageBehavior;
        //friend class AfterCreateImageBehavior;

        Image(Context* ctx);
        virtual ~Image();

        RenderTargetType getType() const override { return m_renderTargetType; }
        void renderTargetType(RenderTargetType type) { m_renderTargetType = type; }

        virtual void create() = 0;
        virtual void as(ImageUsageFlags usage) = 0;
        virtual void bind(Context* ctx) {}

        Format format() const { return m_behavior->format(); }
        Samples samples() const { return m_behavior->samples(); }

        virtual Image* getImage() const { return nullptr; }
        virtual std::vector<Image*> getTargetImages() = 0;
        virtual RHI::RenderTargetView* currentTargetView() = 0;
        virtual RHI::TextureView* currentTextureView() = 0;
        virtual RHI::DepthStencilView* currentDepthStencilView() = 0;
        int width() const { return m_width; }
        int height() const { return m_height; }
        void width(int width) { m_width = width; }
        void height(int height) { m_height = height; }
        void format(Format format) { m_format = format; }
        void samples(Samples samples) { m_samples = samples; }
    protected:
        ImageBehavior* m_behavior;
        Context* m_ctx;
        int m_width;
        int m_height;
        Format m_format;
        Samples m_samples;
        RenderTargetType m_renderTargetType;
    };
} // namespace FCT

#endif //FCT_IMAGE_H