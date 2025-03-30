//
// Created by Administrator on 2025/3/26.
//

#include "../FCTAPI.h"

namespace FCT
{
    namespace RHI
    {
        VK_PassGroup::VK_PassGroup(VK_Context* ctx) : m_createInfo{}
        {
            m_ctx = ctx;
        }

        uint32_t VK_PassGroup::getImageIndex(FCT::Image* image)
        {
            return m_imageIndices[image];
        }

        void VK_PassGroup::create()
        {
            collectAttachments();
            for (auto& pass : m_passes)
            {
                pass->create(this);
            }
            m_createInfo.attachmentCount = m_attachments.size();
            m_createInfo.dependencyCount = 0;
            m_createInfo.pAttachments = m_attachments.data();
            m_createInfo.pDependencies = nullptr;
            collectSubpasses();
            m_createInfo.pSubpasses = m_subpasses.data();
            m_createInfo.subpassCount = m_subpasses.size();
            m_renderPass = m_ctx->device().createRenderPass(m_createInfo);
        }

        void VK_PassGroup::collectSubpasses()
        {
            for (auto& srcPass : m_passes)
            {
                auto pass = (VK_Pass*&)(srcPass);
                m_subpasses.push_back(pass->getDescription());
            }
        }

        void VK_PassGroup::collectAttachments()
        {
            for (auto& pass : m_passes)
            {
                for (auto& image : pass->renderTargets())
                {
                    vk::AttachmentDescription desc;
                    desc.format = ToVkFormat(image->format());
                    desc.samples = ToVkSampleCount(image->samples());
                    desc.loadOp = vk::AttachmentLoadOp::eLoad;
                    desc.storeOp = vk::AttachmentStoreOp::eStore;
                    desc.initialLayout = vk::ImageLayout::eUndefined;
                    //todo:优化initialLayout
                    if (image->getType() == RenderTargetType::WindowTarget && !m_nextPassGroup.size())
                    {
                        desc.finalLayout = vk::ImageLayout::ePresentSrcKHR;
                    } else
                    {
                        desc.finalLayout = vk::ImageLayout::eGeneral;
                        //Todo: 根据nextPassGroup来优化finalLayout，或者由外部手动指定finalLayout
                    }
                    desc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
                    desc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
                    m_imageIndices[image] = m_attachments.size();
                    m_attachments.push_back(desc);
                }
            }
        }
    }
}
