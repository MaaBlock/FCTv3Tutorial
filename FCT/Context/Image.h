//
// Created by Administrator on 2025/3/5.
//
#include "../MutilThreadBase/RefCount.h"
#include "./IRenderTarget.h"
#include "./Format.h"
#include "../RHI/Image.h"
#include "../RHI/RenderTargetView.h"

#ifndef FCT_IMAGE_H
#define FCT_IMAGE_H

namespace FCT {
    class Context;
    enum class ImageUsage : unsigned int
    {
        RenderTarget = 0x1,
        DepthStencil = 0x2,
        Texture      = 0x4,
    };
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
    };
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
    class AfterCreateImageBehavior : public ImageBehavior
    {
    public:
        AfterCreateImageBehavior(Image* image);
        Format format() const;
        Samples samples() const;

    };
    class Image : public RefCount, public IRenderTarget {
    public:
        friend class BeforeCreateImageBehavior;
        friend class AfterCreateImageBehavior;
        Image(Context* ctx);
        RenderTargetType getType() const override
        {
            return m_renderTargetType;
        }
        void renderTargetType(RenderTargetType type)
        {
            m_renderTargetType = type;
        }
        virtual ~Image();
        void create();
        void create(RHI::Image* image);
        void as(ImageUsageFlags usage);
        void bind(Context* ctx)
        {

        }
        Format format() const
        {
            return m_behavior->format();
        }
        Samples samples() const
        {
            return m_behavior->samples();
        }
        Image* getImage() const
        {
            return nullptr;
        }
        std::vector<Image*> getTargetImages() override
        {
            if (m_rtv)
            {
                std::vector<Image*> ret;
                ret.push_back(this);
                return ret;
            }
            return {};
        }
    protected:
        ImageBehavior* m_behavior;
        RHI::Image* m_image;
        RHI::RenderTargetView* m_rtv;
        Context* m_ctx;
        int m_width;
        int m_height;
        Format m_format;
        Samples m_samples;
        RenderTargetType m_renderTargetType;
    };


} // namespace FCT

#endif //FCT_IMAGE_H