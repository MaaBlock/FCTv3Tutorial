//
// Created by Administrator on 2025/3/26.
//

#include "../FCTAPI.h"

namespace FCT
{
    namespace RHI
    {
        VK_PassGroup::VK_PassGroup(VK_Context* ctx) : m_createInfo{},m_beginInfo{},m_framebufferInfo{} // , m_framebuffer()
        {
            m_ctx = ctx;
        }
/*
        uint32_t VK_PassGroup::getImageIndex(FCT::Image* image)
        {
            return m_imageIndices[image];
        }
*/
        /*
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

            std::vector<vk::SubpassDependency> dependencies;

            vk::SubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            dependency.srcAccessMask = vk::AccessFlags();
            dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;;
            dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
            dependencies.push_back(dependency);

            vk::SubpassDependency outDependency{};
            outDependency.srcSubpass = 0;
            outDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
            outDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            outDependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
            outDependency.dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
            outDependency.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
            dependencies.push_back(outDependency);

            m_createInfo.setDependencies(dependencies);

            collectSubpasses();
            m_createInfo.pSubpasses = m_subpasses.data();
            m_createInfo.subpassCount = m_subpasses.size();
            m_renderPass = m_ctx->device().createRenderPass(m_createInfo);
            m_beginInfo.renderPass = m_renderPass;
            m_framebufferInfo.renderPass = m_renderPass;
        }
*/
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

            std::vector<vk::SubpassDependency> dependencies;

            bool hasDepthStencil = !m_depthAttachments.empty();

            vk::SubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;

            if (hasDepthStencil) {
                dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
                                          vk::PipelineStageFlagBits::eEarlyFragmentTests;
            } else {
                dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            }

            dependency.srcAccessMask = vk::AccessFlags();

            if (hasDepthStencil) {
                dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
                                          vk::PipelineStageFlagBits::eEarlyFragmentTests;
                dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite |
                                           vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            } else {
                dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
            }

            dependencies.push_back(dependency);

            vk::SubpassDependency outDependency{};
            outDependency.srcSubpass = 0;
            outDependency.dstSubpass = VK_SUBPASS_EXTERNAL;

            if (hasDepthStencil) {
                outDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
                                             vk::PipelineStageFlagBits::eLateFragmentTests;
                outDependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite |
                                              vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            } else {
                outDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                outDependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
            }

            outDependency.dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
            outDependency.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
            dependencies.push_back(outDependency);

            if (m_subpasses.size() > 1) {
                for (size_t i = 0; i < m_subpasses.size() - 1; i++) {
                    vk::SubpassDependency subpassDependency{};
                    subpassDependency.srcSubpass = i;
                    subpassDependency.dstSubpass = i + 1;

                    subpassDependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                    subpassDependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

                    if (hasDepthStencil) {
                        subpassDependency.srcStageMask |= vk::PipelineStageFlagBits::eLateFragmentTests;
                        subpassDependency.srcAccessMask |= vk::AccessFlagBits::eDepthStencilAttachmentWrite;
                    }

                    subpassDependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                    subpassDependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

                    if (hasDepthStencil) {
                        subpassDependency.dstStageMask |= vk::PipelineStageFlagBits::eEarlyFragmentTests;
                        subpassDependency.dstAccessMask |= vk::AccessFlagBits::eDepthStencilAttachmentWrite;
                    }

                    dependencies.push_back(subpassDependency);
                }
            }

            m_createInfo.setDependencies(dependencies);

            collectSubpasses();
            m_createInfo.pSubpasses = m_subpasses.data();
            m_createInfo.subpassCount = m_subpasses.size();

            try {
                m_renderPass = m_ctx->device().createRenderPass(m_createInfo);
            } catch (const std::exception& e) {
                ferr << "Failed to create render pass: " << e.what() << std::endl;
                throw;
            }

            m_beginInfo.renderPass = m_renderPass;
            m_framebufferInfo.renderPass = m_renderPass;
        }
        uint32_t VK_PassGroup::getPassIndex(Pass* pass)
        {
            return m_passIndices[pass];
        }
#ifdef FCT_DEPRECATED
        void VK_PassGroup::beginSubmit(CommandBuffer* cmdBuf)
        {
            collectImageViews();
            if (m_framebuffers[cmdBuf])
            {
                m_ctx->device().destroyFramebuffer(m_framebuffers[cmdBuf]);
                m_framebuffers[cmdBuf] = nullptr;
            }
            /*
            if (m_framebuffer)
            {
                m_ctx->device().destroyFramebuffer(m_framebuffer);
                m_framebuffer = nullptr;
            }*/

            m_framebufferInfo.attachmentCount = m_framebufferViews.size();
            m_framebufferInfo.pAttachments = m_framebufferViews.data();
            m_framebufferInfo.width = m_targetAttachments.rbegin()->second.image->width();
            m_framebufferInfo.height = m_targetAttachments.rbegin()->second.image->height();
            m_framebufferInfo.layers = 1;
            m_framebufferInfo.renderPass = m_renderPass;
            m_framebuffers[cmdBuf] = m_ctx->device().createFramebuffer(m_framebufferInfo);

            m_beginInfo.framebuffer = m_framebuffers[cmdBuf];
            m_beginInfo.renderArea.offset.setX(0).setY(0);
            m_beginInfo.renderArea.extent.width =
                m_targetAttachments.rbegin()->second.image->width();
            m_beginInfo.renderArea.extent.height =
                m_targetAttachments.rbegin()->second.image->height();

            m_clearValues =  std::vector<vk::ClearValue>(m_attachments.size());
            for (size_t i = 0; i < m_clearValues.size(); i++) {
                m_clearValues[i].setColor(vk::ClearColorValue(std::array<float, 4>{1.0f, 1.0f, 1.0f, 1.0f}));
            }

            m_beginInfo.setClearValues(m_clearValues);

            auto vkCmdBuf = static_cast<VK_CommandBuffer*>(cmdBuf);
            vkCmdBuf->commandBuffer().beginRenderPass(m_beginInfo,vk::SubpassContents::eInline);
        }
#endif
        void VK_PassGroup::beginSubmit(CommandBuffer* cmdBuf)
        {
            collectImageViews();
            if (m_framebuffers[cmdBuf])
            {
                m_ctx->device().destroyFramebuffer(m_framebuffers[cmdBuf]);
                m_framebuffers[cmdBuf] = nullptr;
            }

            m_framebufferInfo.attachmentCount = m_framebufferViews.size();
            m_framebufferInfo.pAttachments = m_framebufferViews.data();

            if (m_targetAttachments.empty() && m_depthAttachments.empty()) {
                ferr << "No attachments for framebuffer!" << std::endl;
                return;
            }

            uint32_t width, height;
            if (!m_targetAttachments.empty()) {
                width = m_targetAttachments.rbegin()->second.image->width();
                height = m_targetAttachments.rbegin()->second.image->height();
            } else {
                width = m_depthAttachments.rbegin()->second.image->width();
                height = m_depthAttachments.rbegin()->second.image->height();
            }

            m_framebufferInfo.width = width;
            m_framebufferInfo.height = height;
            m_framebufferInfo.layers = 1;
            m_framebufferInfo.renderPass = m_renderPass;

            try {
                m_framebuffers[cmdBuf] = m_ctx->device().createFramebuffer(m_framebufferInfo);
            } catch (const std::exception& e) {
                ferr << "Failed to create framebuffer: " << e.what() << std::endl;
                throw;
            }

            m_beginInfo.framebuffer = m_framebuffers[cmdBuf];
            m_beginInfo.renderArea.offset.setX(0).setY(0);
            m_beginInfo.renderArea.extent.width = width;
            m_beginInfo.renderArea.extent.height = height;

            m_clearValues = std::vector<vk::ClearValue>(m_attachments.size());
            for (size_t i = 0; i < m_clearValues.size(); i++) {
                if (m_depthAttachments.find(i) != m_depthAttachments.end()) {
                    m_clearValues[i].setDepthStencil(vk::ClearDepthStencilValue(1.0f, 0));
                } else {
                    m_clearValues[i].setColor(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}));
                }
            }

            m_beginInfo.setClearValues(m_clearValues);

            auto vkCmdBuf = static_cast<VK_CommandBuffer*>(cmdBuf);
            vkCmdBuf->commandBuffer().beginRenderPass(m_beginInfo, vk::SubpassContents::eInline);
        }

        void VK_PassGroup::endSubmit(CommandBuffer* cmdBuf)
        {
            auto vkCmdBuf = static_cast<VK_CommandBuffer*>(cmdBuf);
            vkCmdBuf->commandBuffer().endRenderPass();
        }

        void VK_PassGroup::collectSubpasses()
        {
            for (auto& srcPass : m_passes)
            {
                auto pass = (VK_Pass*&)(srcPass);
                m_passIndices[pass] = m_subpasses.size();
                m_subpasses.push_back(pass->getDescription());
            }
        }
#ifdef FCT_DEPRECATED
        void VK_PassGroup::collectAttachments()
        {
            for (auto& pass : m_passes)
            {
                for (auto& targetPair : pass->renderTargets())
                {
                    auto& image = targetPair.second;
                    vk::AttachmentDescription desc;
                    desc.format = ToVkFormat(image->format());
                    desc.samples = ToVkSampleCount(image->samples());
                    desc.loadOp = vk::AttachmentLoadOp::eClear;
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
                    //m_imageIndices[image] = m_attachments.size();
                    /*m_attachmentSlots[m_attachments.size()] = AttachmentSlot(
                        image,
                        ImageUsage::RenderTarget,
                        targetPair.first);*/
                    m_targetAttachments[m_attachments.size()] = AttachmentSlot(
                        image,
                        ImageUsage::RenderTarget,
                        targetPair.first);
                    auto vkPass = static_cast<VK_Pass*>(pass);
                    vkPass->targetAttachmentIndices()[targetPair.first] = m_attachments.size();
                    m_attachments.push_back(desc);
                }
            }
        }

        void VK_PassGroup::collectImageViews() // 注意，顺序要保持和collectAttachments一样
        {
            m_framebufferViews.clear();
            for (auto& pass : m_passes)
            {
                for (auto& targetPair : pass->renderTargets())
                {
                    auto& image = targetPair.second;
                    m_framebufferViews.push_back(
                        static_cast<VK_RenderTargetView*>(image->currentTargetView())->view());
                //    m_attachments.push_back(desc);
                }
            }
        }
#endif

     void VK_PassGroup::collectAttachments()
        {
            // 首先收集所有渲染目标
            for (auto& pass : m_passes)
            {
                for (auto& targetPair : pass->renderTargets())
                {
                    auto& image = targetPair.second;
                    vk::AttachmentDescription desc;
                    desc.format = ToVkFormat(image->format());
                    desc.samples = ToVkSampleCount(image->samples());
                    desc.loadOp = vk::AttachmentLoadOp::eClear;
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

                    m_targetAttachments[m_attachments.size()] = AttachmentSlot(
                        image,
                        ImageUsage::RenderTarget,
                        targetPair.first);
                    auto vkPass = static_cast<VK_Pass*>(pass);
                    vkPass->targetAttachmentIndices()[targetPair.first] = m_attachments.size();
                    m_attachments.push_back(desc);
                }

                auto vkPass = static_cast<VK_Pass*>(pass);
                FCT::Image* depthStencil = vkPass->depthStencil();
                if (depthStencil)
                {
                    vk::AttachmentDescription desc;
                    desc.format = ToVkFormat(depthStencil->format());
                    desc.samples = ToVkSampleCount(depthStencil->samples());

                    desc.loadOp = vk::AttachmentLoadOp::eClear;
                    desc.storeOp = vk::AttachmentStoreOp::eStore;

                    if (desc.format == vk::Format::eD24UnormS8Uint ||
                        desc.format == vk::Format::eD32SfloatS8Uint ||
                        desc.format == vk::Format::eD16UnormS8Uint)
                    {
                        desc.stencilLoadOp = vk::AttachmentLoadOp::eClear;
                        desc.stencilStoreOp = vk::AttachmentStoreOp::eStore;
                    }
                    else
                    {
                        desc.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
                        desc.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
                    }

                    desc.initialLayout = vk::ImageLayout::eUndefined;
                    desc.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

                    m_depthAttachments[m_attachments.size()] = AttachmentSlot(
                        depthStencil,
                        ImageUsage::DepthStencil,
                        0);

                    vkPass->setDepthStencilAttachmentIndex(m_attachments.size());

                    m_attachments.push_back(desc);
                }
            }
        }

        void VK_PassGroup::collectImageViews() // 注意，顺序要保持和collectAttachments一样
        {
            m_framebufferViews.clear();

            // 收集渲染目标视图
            for (auto& pass : m_passes)
            {
                for (auto& targetPair : pass->renderTargets())
                {
                    auto& image = targetPair.second;
                    m_framebufferViews.push_back(
                        static_cast<VK_RenderTargetView*>(image->currentTargetView())->view());
                }

                auto vkPass = static_cast<VK_Pass*>(pass);
                FCT::Image* depthStencil = vkPass->depthStencil();
                if (depthStencil)
                {
                    auto dsv = depthStencil->currentDepthStencilView();
                    if (dsv)
                    {
                        auto vkDsv = static_cast<VK_DepthStencilView*>(dsv);
                        m_framebufferViews.push_back(vkDsv->getImageView());
                    }
                    else
                    {
                        ferr << "Depth stencil image has no depth stencil view!" << std::endl;
                    }
                }
            }
        }
    }
}
