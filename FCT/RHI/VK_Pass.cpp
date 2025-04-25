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

            if (m_depthStencil) {
                m_depthStencilRef.attachment = m_depthStencilAttachmentIndex;
                m_depthStencilRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

                m_desc.pDepthStencilAttachment = &m_depthStencilRef;

                fout << "Pass using depth stencil attachment with index: "
                     << m_depthStencilAttachmentIndex << std::endl;
            } else {
                m_desc.pDepthStencilAttachment = nullptr;
            }

            m_group = group;
        }


       void VK_Pass::beginSubmit(CommandBuffer* cmdBuf)
        {
            if (!m_clearValue.types) {
                return;
            }

            auto vkCmdBuf = static_cast<VK_CommandBuffer*>(cmdBuf);
            std::vector<vk::ClearAttachment> clearAttachments;

            if (m_clearValue.types & ClearType::color) {
                for (const auto& [slot, attachmentIndex] : m_targetAttachmentIndices) {
                    if (slot < m_renderTargetRefs.size() && m_renderTargetRefs[slot].attachment != VK_ATTACHMENT_UNUSED) {
                        vk::ClearAttachment clearAttachment;
                        clearAttachment.aspectMask = vk::ImageAspectFlagBits::eColor;
                        clearAttachment.colorAttachment = slot;
                        clearAttachment.clearValue.color = vk::ClearColorValue(std::array<float, 4>{
                            m_clearValue.color.x,
                            m_clearValue.color.y,
                            m_clearValue.color.z,
                            m_clearValue.color.w
                        });
                        clearAttachments.push_back(clearAttachment);
                    }
                }
            }

            if (m_depthStencil && m_depthStencilAttachmentIndex != UINT32_MAX &&
                ((m_clearValue.types & ClearType::depth) || (m_clearValue.types & ClearType::stencil))) {

                vk::ClearAttachment clearAttachment;
                clearAttachment.aspectMask = vk::ImageAspectFlagBits::eNone;

                if (m_clearValue.types & ClearType::depth) {
                    clearAttachment.aspectMask |= vk::ImageAspectFlagBits::eDepth;
                }

                if (m_clearValue.types & ClearType::stencil) {
                    clearAttachment.aspectMask |= vk::ImageAspectFlagBits::eStencil;
                }

                clearAttachment.clearValue.depthStencil = vk::ClearDepthStencilValue(
                    m_clearValue.depth,
                    m_clearValue.stencil
                );

                clearAttachments.push_back(clearAttachment);
                }

            if (!clearAttachments.empty()) {
                vk::ClearRect clearRect;
                clearRect.rect.offset = vk::Offset2D(0, 0);

                uint32_t width = 0, height = 0;
                if (!m_renderTargets.empty()) {
                    auto& firstTarget = m_renderTargets.begin()->second;
                    width = firstTarget->width();
                    height = firstTarget->height();
                } else if (m_depthStencil) {
                    width = m_depthStencil->width();
                    height = m_depthStencil->height();
                }

                clearRect.rect.extent = vk::Extent2D(width, height);
                clearRect.baseArrayLayer = 0;
                clearRect.layerCount = 1;

                vkCmdBuf->commandBuffer().clearAttachments(clearAttachments, clearRect);
            }
        }

        void VK_Pass::endSubmit()
        {

        }
    }
}
