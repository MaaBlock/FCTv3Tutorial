//
// Created by Administrator on 2025/3/27.
//
#include "./RenderTargetView.h"
#ifndef VK_RENDERTARGETVIEW_H
#define VK_RENDERTARGETVIEW_H

namespace FCT
{
    namespace RHI
    {
        class VK_RenderTargetView : public RenderTargetView {
        public:
            VK_RenderTargetView(VK_Context* ctx);
            void create() override;
        protected:
            VK_Context* m_ctx;
            vk::ImageView m_view;
        };


    }
}


#endif //VK_RENDERTARGETVIEW_H
