//
// Created by Administrator on 2025/3/23.
//

#include "../FCTAPI.h"

namespace FCT
{

    VK_ViewportState::VK_ViewportState(VK_Context* ctx) : m_createInfo{}
    {
        m_ctx = ctx;
    }

    void VK_ViewportState::bind()
    {
    }

    void VK_ViewportState::unbind()
    {
    }

    void VK_ViewportState::create()
    {
        m_viewport.setX(m_viewportPosition.x)
                  .setY(m_viewportPosition.y)
                  .setWidth(m_viewportSize.x)
                  .setHeight(m_viewportSize.y)
                  .setMinDepth(m_minDepth)
                  .setMaxDepth(m_maxDepth);
        m_scissor.extent.setHeight(m_scissorSize.y)
                        .setWidth(m_scissorSize.x);
        m_scissor.offset.setX(m_scissorPosition.x)
                         .setY(m_scissorPosition.y);
        m_createInfo.setScissorCount(1)
                    .setPScissors(&m_scissor)
                    .setPViewports(&m_viewport)
                    .setViewportCount(1);
    }
}
