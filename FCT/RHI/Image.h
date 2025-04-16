//
// Created by Administrator on 2025/3/27.
//
#include "../MutilThreadBase/RefCount.h"
#include "../Base/Flags.h"
#include "../Context/Format.h"

#ifndef FCT_RHI_IMAGE_H
#define FCT_RHI_IMAGE_H
namespace FCT
{
    enum class ImageUsage : unsigned int
    {
        RenderTarget = 0x1,
        DepthStencil = 0x2,
        Texture      = 0x4,
    };
    struct ImageInitData
    {
        void* data;
        size_t size;
    };
    using ImageUsageFlags = Flags<ImageUsage>;
    namespace RHI
    {
        class Image : public RefCount
        {
        public:
            Image()
            {
                m_initData.data = nullptr;
                m_initData.size = 0;
            }
            ~Image() = default;
            Format format() const
            {
                return m_format;
            }
            void format(Format format)
            {
                m_format = format;
            }
            Samples samples() const
            {
                return m_samples;
            }
            void samples(Samples samples)
            {
                m_samples = samples;
            }
            void width(uint32_t width)
            {
                m_width = width;
            }
            void height(uint32_t height)
            {
                m_height = height;
            }
            uint32_t width() const
            {
                return m_width;
            }
            void initData(void* data, size_t size)
            {
                m_initData.data = data;
                m_initData.size = size;
            }
            void initData(ImageInitData data)
            {
                m_initData = data;
            }
            uint32_t height() const
            {
                return m_height;
            }
            virtual void create() = 0;
            void usage(ImageUsageFlags usage)
            {
                m_usage = usage;
            }
        protected:
            Format m_format;
            Samples m_samples;
            uint32_t m_width;
            uint32_t m_height;
            ImageUsageFlags m_usage;
            ImageInitData m_initData;
        };
    }
}
#endif //FCT_RHI_IMAGE_H
