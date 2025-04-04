//
// Created by Administrator on 2025/3/26.
//

#include "./PassGroup.h"
#ifndef VK_PASSGROUP_H
#define VK_PASSGROUP_H

namespace FCT
{
     namespace RHI
    {
        struct AttachmentSlot
        {
            FCT::Image* image;
            ImageUsage usage;
            uint32_t slot;
            AttachmentSlot() = default;
            AttachmentSlot(FCT::Image* image, ImageUsage usage, uint32_t slot) : image(image), usage(usage), slot(slot) {}
        };
        class VK_PassGroup : public PassGroup {
        public:
            VK_PassGroup(VK_Context* ctx);
            //uint32_t getImageIndex(FCT::Image* image);
            void create();
            vk::RenderPass getRenderPass() const { return m_renderPass; }
            uint32_t getPassIndex(Pass* pass);
            void beginSubmit(CommandBuffer* cmdBuf) override;
            void endSubmit(CommandBuffer* cmdBuf) override;
        private:
            void collectSubpasses();
            void collectAttachments();
            void collectImageViews();
            //std::map<FCT::Image*,uint32_t> m_imageIndices;
            //std::map<uint32_t,AttachmentSlot> m_attachmentSlots;
            std::map<uint32_t,AttachmentSlot> m_targetAttachments;
            std::map<Pass*,uint32_t> m_passIndices;
            std::vector<vk::AttachmentDescription> m_attachments;
            std::vector<vk::SubpassDescription> m_subpasses;
            std::vector<vk::ImageView> m_framebufferViews;
            VK_Context* m_ctx;
            vk::RenderPassCreateInfo m_createInfo;
            vk::RenderPass m_renderPass;
            IRenderTarget* m_target;
            vk::RenderPassBeginInfo m_beginInfo;
            vk::FramebufferCreateInfo m_framebufferInfo;
            vk::Framebuffer m_framebuffer;
            std::vector<vk::ClearValue> m_clearValues;

            //std::vector<FCT::Image*> m_targets;
        };
    }
}


#endif //VK_PASSGROUP_H
