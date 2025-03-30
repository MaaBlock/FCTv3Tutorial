//
// Created by Administrator on 2025/3/23.
//
#include "../ThirdParty.h"
#include "./ViewportState.h"
#ifndef VK_VIEWPORTSTATE_H
#define VK_VIEWPORTSTATE_H

namespace FCT
{
    class VK_Context;
    class VK_ViewportState : public ViewportState {
    public:
        VK_ViewportState(VK_Context* ctx);
        void bind() override;
        void unbind() override;
        void create() override;
    private:
        vk::Rect2D m_scissor;
        vk::Viewport m_viewport;
        vk::PipelineViewportStateCreateInfo m_createInfo;
        VK_Context* m_ctx;
    };

}

#endif //VK_VIEWPORTSTATE_H
