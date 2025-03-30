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
        class VK_PassGroup : public PassGroup {
        public:
            VK_PassGroup(VK_Context* ctx);
            uint32_t getImageIndex(FCT::Image* image);
            void create();
        private:
            void collectSubpasses();
            void collectAttachments();
            std::map<FCT::Image*,uint32_t> m_imageIndices;
            std::vector<vk::AttachmentDescription> m_attachments;
            std::vector<vk::SubpassDescription> m_subpasses;
            VK_Context* m_ctx;
            vk::RenderPassCreateInfo m_createInfo;
            vk::RenderPass m_renderPass;
            IRenderTarget* m_target;
        };


    }
}


#endif //VK_PASSGROUP_H
