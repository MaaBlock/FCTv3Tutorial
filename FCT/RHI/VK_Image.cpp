//
// Created by Administrator on 2025/3/27.
//

#include "../FCTAPI.h"
namespace FCT {
    namespace RHI {
        VK_Image::VK_Image(VK_Context* ctx)
        {
            m_ctx = ctx;
            m_owns = false;
        }

        void VK_Image::create()
        {
        }

        void VK_Image::create(vk::Image image)
        {
            m_image = image;
            m_owns = false;
        }

        vk::Image VK_Image::getVkImage()
        {
            return m_image;
        }
    }
}