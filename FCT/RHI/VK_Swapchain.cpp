#include "../FCTAPI.h"


namespace FCT {
    namespace RHI
    {
        VK_Swapchain::VK_Swapchain(VK_Context *ctx) {
            m_ctx = ctx;
            m_swapchain = nullptr;
            m_fctImage = nullptr;
            m_target = nullptr;
            m_prensentFinshSemphore = nullptr;
            m_needRecreated = false;
        }

        VK_Swapchain::~VK_Swapchain() {

        }

        void VK_Swapchain::create(vk::SurfaceKHR surface,uint32_t width,uint32_t height) {
            if (m_fctImages.size())
            {
                for (auto image : m_fctImages)
                {
                    image->release();
                }
                m_fctImages.clear();
            }
            //FCT_SAFE_RELEASE(m_target);
            //FCT_SAFE_RELEASE(m_fctImage);

            auto phyDc = m_ctx->getPhysicalDevice();
            auto dc = m_ctx->getDevice();

            m_surface = surface;

            auto graphicsQueueFamily = m_ctx->getGraphicsQueueFamily();
            m_presentQueueFamilyIndex = UINT32_MAX;

            auto queueFamilyCount = phyDc.getQueueFamilyProperties().size();
            for (uint32_t i = 0; i < queueFamilyCount; i++) {
                if (phyDc.getSurfaceSupportKHR(i, surface)) {
                    m_presentQueueFamilyIndex = i;
                    break;
                }
            }

            if (m_presentQueueFamilyIndex == UINT32_MAX) {
                m_presentQueueFamilyIndex = graphicsQueueFamily;
            }

            m_presentQueue = dc.getQueue(m_presentQueueFamilyIndex, 0);

            auto capabilities = phyDc.getSurfaceCapabilitiesKHR(surface);
            auto formats = phyDc.getSurfaceFormatsKHR(surface);
            auto presentModes = phyDc.getSurfacePresentModesKHR(surface);

            auto format = chooseSwapSurfaceFormat(formats);
            auto presentMode = chooseSwapPresentMode(presentModes);

            m_imageFormat = format.format;

            vk::Extent2D extent(
                    std::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
                    std::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
            );

            auto imageCount = std::clamp(2u, capabilities.minImageCount, capabilities.maxImageCount);

            vk::SwapchainCreateInfoKHR createInfo{};
            createInfo.setSurface(surface)
                      .setMinImageCount(std::clamp(2u, capabilities.minImageCount, capabilities.maxImageCount))
                      .setImageFormat(format.format)
                      .setImageColorSpace(format.colorSpace)
                      .setImageExtent(extent)
                      .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                      .setPreTransform(capabilities.currentTransform)
                      .setPresentMode(presentMode)
                      .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                      .setImageArrayLayers(1)
                      .setClipped(true);
            uint32_t queueFamilyIndices[] = {graphicsQueueFamily, m_presentQueueFamilyIndex};
            if (graphicsQueueFamily != m_presentQueueFamilyIndex) {
                createInfo.setImageSharingMode(vk::SharingMode::eConcurrent)
                        .setQueueFamilyIndexCount(2)
                        .setPQueueFamilyIndices(queueFamilyIndices);
            } else {
                createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
            }

            vk::SwapchainKHR oldSwapchain = m_swapchain;

            if (oldSwapchain) {
                createInfo.setOldSwapchain(oldSwapchain);
            }

            m_swapchain = dc.createSwapchainKHR(createInfo);

            m_images = dc.getSwapchainImagesKHR(m_swapchain);

            for (auto image : m_images)
            {
                m_fctImages.push_back(
                    ([this,image]()
                    {
                        auto ret = (new VK_Image(m_ctx));
                        ret->samples(getSamples());
                        ret->format(getFormat());
                        ret->width(m_width);
                        ret->height(m_height);
                        ret->create(image);
                        return ret;
                    }).operator()());
            }
            if (m_fctImage)
            {
                m_fctImage->create(m_fctImages);
            } else
            {
                m_fctImage = new MutilBufferImage(m_ctx);
                m_fctImage->renderTargetType(RenderTargetType::WindowTarget);
                m_fctImage->as(ImageUsage::RenderTarget);
                m_fctImage->create(m_fctImages);
            }
            if (m_target)
            {

            } else
            {
                m_target = new ImageRenderTarget(m_ctx);
                m_target->renderTargetType(RenderTargetType::WindowTarget);
                m_target->bindTarget(m_fctImage);
            }
            //createImageViews();
            /*
            if (!m_prensentFinshSemphore)
            {
                m_prensentFinshSemphore = static_cast<VK_Semaphore*>(m_ctx->createSemaphore());
                m_prensentFinshSemphore->create();
            }*/

            if (oldSwapchain) {
                dc.destroySwapchainKHR(oldSwapchain);
                acquireFirstImage();
            }
        }

        void VK_Swapchain::create()
        {
            create(*static_cast<vk::SurfaceKHR*>(m_nativeHandle),m_width,m_height);
        }

        vk::SurfaceFormatKHR VK_Swapchain::chooseSwapSurfaceFormat(
                const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
            for (const auto &availableFormat : availableFormats) {
                if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
                    availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                    return availableFormat;
                    }
            }
            return availableFormats[0];
        }

        vk::PresentModeKHR VK_Swapchain::chooseSwapPresentMode(
                const std::vector<vk::PresentModeKHR> &availablePresentModes) {
            for (const auto &availablePresentMode : availablePresentModes) {
                if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
                    return availablePresentMode;
                }
            }
            return vk::PresentModeKHR::eFifo;
        }

        void VK_Swapchain::destroy() {

        }


        uint32_t VK_Swapchain::getCurrentImageIndex() const {
            return m_currentImageIndex;
        }
        void VK_Swapchain::present()
        {
            auto dc = m_ctx->getDevice();
            if (processRecreate(true))
            {
                for (auto semaphore : m_renderFinshSemaphores)
                {
                    semaphore->create();
                }
                return;
            }

            std::vector<vk::Semaphore> semaphores;
            for (auto semaphore : m_renderFinshSemaphores)
            {
                semaphores.push_back(static_cast<VK_Semaphore*>(semaphore)->semaphore());
            }
            vk::PresentInfoKHR presentInfo{};
            presentInfo.setWaitSemaphores(semaphores)
                    .setSwapchainCount(1)
                    .setPSwapchains(&m_swapchain)
                    .setPImageIndices(&m_currentImageIndex);
            try {
                auto result = m_presentQueue.presentKHR(presentInfo);
                if (result == vk::Result::eSuboptimalKHR) {
                    m_needRecreated = true;
                }
            } catch (vk::OutOfDateKHRError& e) {
                m_needRecreated = true;
                processRecreate();
                for (auto fence : m_renderFinshFences)
                {
                    fence->waitFor();
                    fence->reset();
                }
                for (auto semaphore : m_renderFinshSemaphores)
                {
                    semaphore->create();
                }
                return;
            } catch (vk::SystemError& e)
            {
                ferr << "fetal error in VK_Swapchain::present()" << e.what() << std::endl;
                return;
            }
            for (auto fence : m_renderFinshFences)
            {
                fence->waitFor();
                fence->reset();
            }
            try {
                auto nextResult = dc.acquireNextImageKHR(
                        m_swapchain,
                        UINT64_MAX,
                        m_prensentFinshSemphore->semaphore(),
                        nullptr,
                        &m_currentImageIndex
                );

                if (nextResult == vk::Result::eSuboptimalKHR) {
                    m_needRecreated = true;
                }
            } catch (vk::OutOfDateKHRError& e) {
                m_needRecreated = true;
                processRecreate();
                return;
            } catch (vk::SystemError& e) {
                ferr << "fetal error in VK_Swapchain::present() - acquireNextImage" << e.what() << std::endl;
                return;
            }

            m_fctImage->changeCurrentIndex(m_currentImageIndex);
        }
        void VK_Swapchain::acquireFirstImage() {
            auto dc = m_ctx->getDevice();
            VK_Fence* fence = nullptr;
            if (m_prensentFinshSemphore)
            {
                m_prensentFinshSemphore->create();
                dc.acquireNextImageKHR(
                        m_swapchain,
                        UINT64_MAX,
                        m_prensentFinshSemphore->semaphore(),
                        nullptr,
                        &m_currentImageIndex
                );
            } else
            {
                fence = new VK_Fence(m_ctx);
                fence->create();
                dc.acquireNextImageKHR(
                        m_swapchain,
                        UINT64_MAX,
                        nullptr,
                        fence->fence(),
                        &m_currentImageIndex
                );
                fence->waitFor();
                fence->release();
            }
            m_fctImage->changeCurrentIndex(m_currentImageIndex);
        }

        void VK_Swapchain::needRecreate()
        {
            m_needRecreated = true;
            m_recreated = false;
            FCT_WAIT_FOR(m_recreated);
        }

        bool VK_Swapchain::processRecreate(bool waitFence)
        {
            auto dc = m_ctx->getDevice();
            if (m_needRecreated)
            {
                if (waitFence)
                {
                    for (auto fence : m_renderFinshFences)
                    {
                        fence->waitFor();
                        fence->reset();
                    }
                }
                create();
                m_needRecreated = false;
                m_recreated = true;
                return true;
            }
            return false;
        }

        vk::Extent2D VK_Swapchain::getExtent() const {
            return m_extent;
        }

        Format VK_Swapchain::getFormat() const
        {
            return FromVkFormat(m_imageFormat);
        }

        Samples VK_Swapchain::getSamples() const
        {
            return Samples::sample_1;
        }

        ImageRenderTarget* VK_Swapchain::getCurrentTarget()
        {
            return m_target;
        }

        void VK_Swapchain::setPresentFinshSemaphore(RHI::Semaphore* semaphore)
        {
            m_prensentFinshSemphore = static_cast<VK_Semaphore*>(semaphore);
        }

        RHI::Semaphore* VK_Swapchain::getImageAvailableSemaphore()
        {
            return m_prensentFinshSemphore;
        }
    }
}

