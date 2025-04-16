//
// Created by Administrator on 2025/4/15.
//
#include "../MutilThreadBase/RefCount.h"
#ifndef FCT_RHI_TEXTUREVIEW_H
#define FCT_RHI_TEXTUREVIEW_H
namespace FCT
{
    namespace RHI
    {
        class TextureView : public RefCount
        {
        public:
            virtual ~TextureView() = default;
            void image(RHI::Image* image)
            {
                m_image = image;
            }
            RHI::Image* image() const
            {
                return m_image;
            }
            virtual void create() = 0;
        protected:
            RHI::Image* m_image;
        };
    }
}
#endif //TEXTUREVIEW_H
