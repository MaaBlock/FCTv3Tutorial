//
// Created by Administrator on 2025/3/28.
//

#include "../FCTAPI.h"
namespace FCT
{
    namespace RHI
    {
        VK_Pass::VK_Pass(VK_Context* ctx)
        {
            m_ctx = ctx;
            m_group = nullptr;
        }

        void VK_Pass::create(PassGroup* srcGroup)
        {
            auto group = static_cast<VK_PassGroup*>(srcGroup);
            m_desc.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

            if (!m_renderTargets.empty()) {
                uint32_t maxSlot = m_renderTargets.rbegin()->first;

                m_renderTargetRefs.resize(maxSlot + 1);

                for (auto& ref : m_renderTargetRefs) {
                    ref.attachment = VK_ATTACHMENT_UNUSED;
                    ref.layout = vk::ImageLayout::eUndefined;
                }

                for (const auto& [slot, target] : m_renderTargets) {
                    vk::AttachmentReference& ref = m_renderTargetRefs[slot];
                    ref.attachment = m_targetAttachmentIndices[slot];
                    ref.layout = vk::ImageLayout::eColorAttachmentOptimal;
                }

                m_desc.colorAttachmentCount = maxSlot + 1;
                m_desc.pColorAttachments = m_renderTargetRefs.data();
            } else {
                m_desc.colorAttachmentCount = 0;
                m_desc.pColorAttachments = nullptr;
            }

            m_group = group;
        }
    }
}
