//
// Created by Administrator on 2025/4/15.
//

#include "../FCTAPI.h"

namespace FCT
{
    namespace RHI
    {
        VK_TextureView::VK_TextureView(VK_Context* ctx)
            : m_ctx(ctx), m_view(nullptr)
        {
        }

        VK_TextureView::~VK_TextureView()
        {
            if (m_view) {
                m_ctx->getDevice().destroyImageView(m_view);
                m_view = nullptr;
            }
        }

        void VK_TextureView::create()
        {
            if (!m_image) {
                ferr << "Cannot create texture view: no image set" << std::endl;
                return;
            }

            VK_Image* vkImage = dynamic_cast<VK_Image*>(m_image);
            if (!vkImage) {
                ferr << "Cannot create texture view: image is not a VK_Image" << std::endl;
                return;
            }

            vk::ImageViewType viewType = vk::ImageViewType::e2D;
            vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor;

            Format format = m_image->format();

            vk::ImageViewCreateInfo viewInfo;
            viewInfo.setImage(vkImage->getVkImage());
            viewInfo.setViewType(viewType);
            viewInfo.setFormat(ToVkFormat(format));

            viewInfo.components.r = vk::ComponentSwizzle::eR;
            viewInfo.components.g = vk::ComponentSwizzle::eG;
            viewInfo.components.b = vk::ComponentSwizzle::eB;
            viewInfo.components.a = vk::ComponentSwizzle::eA;

            viewInfo.subresourceRange.aspectMask = aspectMask;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            try {
                m_view = m_ctx->getDevice().createImageView(viewInfo);
            }
            catch (const std::exception& e) {
                ferr << "Failed to create texture view: " << e.what() << std::endl;
                m_view = nullptr;
            }
        }
    }
}