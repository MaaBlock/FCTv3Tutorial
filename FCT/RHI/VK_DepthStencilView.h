//
// Created by Administrator on 2025/4/16.
//

#include "../ThirdParty.h"
#include "./DepthStencilView.h"
#include "./VK_Image.h"
#include "../Context/VK_Context.h"

#ifndef VK_DEPTHSTENCILVIEW_H
#define VK_DEPTHSTENCILVIEW_H

namespace FCT
{
    namespace RHI
    {
        class VK_DepthStencilView : public DepthStencilView
        {
        public:
            VK_DepthStencilView(VK_Context* ctx);
            ~VK_DepthStencilView() override;
            bool create() override;
            vk::ImageView getImageView() const { return m_view; }

        private:
            VK_Context* m_ctx;
            vk::ImageView m_view;
        };
    }
}

#endif //VK_DEPTHSTENCILVIEW_H