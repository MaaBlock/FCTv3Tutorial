//
// Created by Administrator on 2025/3/27.
//
#include "../FCTAPI.h"
namespace FCT
{
    namespace RHI
    {
        VK_RenderTargetView::VK_RenderTargetView(VK_Context* ctx)
        {
            m_ctx = ctx;
        }

        void VK_RenderTargetView::create()
        {

            vk::ImageViewCreateInfo createInfo{};
            createInfo.setImage(dynamic_cast<VK_Image*>(m_image)->getVkImage())
                      .setViewType(vk::ImageViewType::e2D)
                      .setFormat(ToVkFormat(m_image->format()))
                      .setComponents(vk::ComponentMapping(
                        vk::ComponentSwizzle::eIdentity,
                        vk::ComponentSwizzle::eIdentity,
                        vk::ComponentSwizzle::eIdentity,
                        vk::ComponentSwizzle::eIdentity))
                      .setSubresourceRange(vk::ImageSubresourceRange(
                        vk::ImageAspectFlagBits::eColor,
                        0, 1, 0, 1));
            m_view = m_ctx->getDevice().createImageView(createInfo);
        }
    }
}
