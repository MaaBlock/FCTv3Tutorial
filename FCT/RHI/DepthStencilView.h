//
// Created by Administrator on 2025/4/16.
//
#include "../MutilThreadBase/RefCount.h"
#ifndef FCT_RHI_DEPTHSTENCILVIEW_H
#define FCT_RHI_DEPTHSTENCILVIEW_H
namespace FCT
{
    namespace RHI
    {
        class DepthStencilView : public RefCount
        {
        public:
            virtual ~DepthStencilView() = default;

            void image(RHI::Image* image)
            {
                m_image = image;
            }

            RHI::Image* image() const
            {
                return m_image;
            }

            virtual bool create() = 0;

        protected:
            RHI::Image* m_image = nullptr;
        };
    }
}
#endif //FCT_RHI_DEPTHSTENCILVIEW_H