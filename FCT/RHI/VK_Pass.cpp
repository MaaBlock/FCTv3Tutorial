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
        }

        void VK_Pass::create(PassGroup* srcGroup)
        {
            auto groutp = static_cast<VK_PassGroup*>(srcGroup);
            m_desc.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
            m_desc.colorAttachmentCount= m_renderTargets.size();
            for (auto target : m_renderTargets)
            {
                vk::AttachmentReference ref;
                ref.layout = vk::ImageLayout::eColorAttachmentOptimal;
                ref.attachment = groutp->getImageIndex(target);
                m_renderTargetRefs.push_back(ref);
            }
            m_desc.pColorAttachments = m_renderTargetRefs.data();
        }
    }
}
