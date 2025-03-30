//
// Created by Administrator on 2025/3/27.
//
#include "../MutilThreadBase/RefCount.h"
#include "../Context/Format.h"
#ifndef FCT_RHI_IMAGE_H
#define FCT_RHI_IMAGE_H
namespace FCT
{
    namespace RHI
    {
        class Image : public RefCount
        {
        public:
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
            uint32_t height() const
            {
                return m_height;
            }
            virtual void create() = 0;
        protected:
            Format m_format;
            Samples m_samples;
            uint32_t m_width;
            uint32_t m_height;
        };
    }
}
#endif //FCT_RHI_IMAGE_H
