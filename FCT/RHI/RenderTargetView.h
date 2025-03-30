//
// Created by Administrator on 2025/3/27.
//
#include "../MutilThreadBase/RefCount.h"
#include "./Image.h"
#ifndef FCT_RHI_RENDERTARGETVIEW_H
#define FCT_RHI_RENDERTARGETVIEW_H
namespace FCT
{
    namespace RHI
    {
        class RenderTargetView : public RefCount
        {
        public:
            virtual ~RenderTargetView() = default;
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
#endif //FCT_RHI_RENDERTARGETVIEW_H
