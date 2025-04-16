//
// Created by Administrator on 2025/4/16.
//

#include "../FCTAPI.h"

namespace FCT
{
    namespace RHI
    {
        VK_DepthStencilView::VK_DepthStencilView(VK_Context* ctx)
            : m_ctx(ctx), m_view(nullptr)
        {
        }

        VK_DepthStencilView::~VK_DepthStencilView()
        {
            if (m_view) {
                m_ctx->getDevice().destroyImageView(m_view);
                m_view = nullptr;
            }
        }

        void VK_DepthStencilView::create()
        {
            if (!m_image) {
                ferr << "Cannot create depth stencil view: no image set" << std::endl;
                return;
            }

            VK_Image* vkImage = dynamic_cast<VK_Image*>(m_image);
            if (!vkImage) {
                ferr << "Cannot create depth stencil view: image is not a VK_Image" << std::endl;
                return;
            }

            vk::ImageViewType viewType = vk::ImageViewType::e2D;
            vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eDepth;

            Format format = m_image->format();
            vk::Format vkFormat = ToVkFormat(format);

            if (vkFormat == vk::Format::eD24UnormS8Uint ||
                vkFormat == vk::Format::eD32SfloatS8Uint ||
                vkFormat == vk::Format::eD16UnormS8Uint) {
                aspectMask |= vk::ImageAspectFlagBits::eStencil;
            }

            vk::ImageViewCreateInfo viewInfo;
            viewInfo.setImage(vkImage->getVkImage());
            viewInfo.setViewType(viewType);
            viewInfo.setFormat(vkFormat);

            viewInfo.components.r = vk::ComponentSwizzle::eIdentity;
            viewInfo.components.g = vk::ComponentSwizzle::eIdentity;
            viewInfo.components.b = vk::ComponentSwizzle::eIdentity;
            viewInfo.components.a = vk::ComponentSwizzle::eIdentity;

            viewInfo.subresourceRange.aspectMask = aspectMask;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            try {
                m_view = m_ctx->getDevice().createImageView(viewInfo);
            }
            catch (const std::exception& e) {
                ferr << "Failed to create depth stencil view: " << e.what() << std::endl;
                m_view = nullptr;
            }
        }
    }
}