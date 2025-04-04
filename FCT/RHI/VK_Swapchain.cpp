#include "../FCTAPI.h"


namespace FCT {
    namespace RHI
    {
        VK_Swapchain::VK_Swapchain(VK_Context *ctx) {
            m_ctx = ctx;
            m_swapchain = nullptr;
            m_fctImage = nullptr;
            m_target = nullptr;
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
                /*for (auto target : m_targets)
                {
                    target->release();
                }
                m_targets.clear();*/
            }
            FCT_SAFE_RELEASE(m_target);
            FCT_SAFE_RELEASE(m_fctImage);

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
                destoryImageViews();
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
                        /*
                        auto target = new ImageRenderTarget(m_ctx);
                        target->renderTargetType(RenderTargetType::WindowTarget);
                        target->addRenderTarget(ret);
                        m_targets.push_back(target);*/
                        return ret;
                    }).operator()());
            }
            m_fctImage = new MutilBufferImage(m_ctx);
            m_fctImage->create(m_fctImages);
            m_fctImage->as(ImageUsage::RenderTarget);
            m_target = new ImageRenderTarget(m_ctx);
            m_target->renderTargetType(RenderTargetType::WindowTarget);
            m_target->bindTarget(m_fctImage);
            //m_target->addRenderTarget(m_fctImage);

            createImageViews();

            if (m_imageAvailableSemaphores.empty()) {
                createSyncObjects();
            }

            if (oldSwapchain) {
                dc.destroySwapchainKHR(oldSwapchain);
            }

            acquireFirstImage();
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


        void VK_Swapchain::createImageViews() {
            auto dc = m_ctx->getDevice();
            m_imageViews.resize(m_images.size());

            for (size_t i = 0; i < m_images.size(); i++) {
                vk::ImageViewCreateInfo createInfo{};
                createInfo.setImage(m_images[i])
                        .setViewType(vk::ImageViewType::e2D)
                        .setFormat(m_imageFormat)
                        .setComponents(vk::ComponentMapping(
                                vk::ComponentSwizzle::eIdentity,
                                vk::ComponentSwizzle::eIdentity,
                                vk::ComponentSwizzle::eIdentity,
                                vk::ComponentSwizzle::eIdentity
                        ))
                        .setSubresourceRange(vk::ImageSubresourceRange(
                                vk::ImageAspectFlagBits::eColor,
                                0, 1, 0, 1
                        ));

                try {
                    m_imageViews[i] = dc.createImageView(createInfo);
                } catch (const vk::SystemError& e) {
                    for (size_t j = 0; j < i; j++) {
                        dc.destroyImageView(m_imageViews[j]);
                    }
                    throw std::runtime_error("Failed to create image views: " + std::string(e.what()));
                }
            }
        }

        void VK_Swapchain::destoryImageViews() {
            auto dc = m_ctx->getDevice();
            for (auto imageView : m_imageViews) {
                if (imageView) {
                    dc.destroyImageView(imageView);
                }
            }
            m_imageViews.clear();
        }

        void VK_Swapchain::createSyncObjects() {
            auto dc = m_ctx->getDevice();

            m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
            m_imagesInFlight.resize(m_images.size(), nullptr);

            vk::SemaphoreCreateInfo semaphoreInfo{};
            vk::FenceCreateInfo fenceInfo{};
            fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                try {
                    m_imageAvailableSemaphores[i] = dc.createSemaphore(semaphoreInfo);
                    m_renderFinishedSemaphores[i] = dc.createSemaphore(semaphoreInfo);
                    m_inFlightFences[i] = dc.createFence(fenceInfo);
                } catch (const vk::SystemError& e) {
                    destroySyncObjects();
                    throw std::runtime_error("Failed to create synchronization objects: " + std::string(e.what()));
                }
            }
        }

        void VK_Swapchain::destroySyncObjects() {
            if (m_ctx) {
                auto dc = m_ctx->getDevice();

                for (size_t i = 0; i < m_imageAvailableSemaphores.size(); i++) {
                    if (m_imageAvailableSemaphores[i]) {
                        dc.destroySemaphore(m_imageAvailableSemaphores[i]);
                    }
                }

                for (size_t i = 0; i < m_renderFinishedSemaphores.size(); i++) {
                    if (m_renderFinishedSemaphores[i]) {
                        dc.destroySemaphore(m_renderFinishedSemaphores[i]);
                    }
                }

                for (size_t i = 0; i < m_inFlightFences.size(); i++) {
                    if (m_inFlightFences[i]) {
                        dc.destroyFence(m_inFlightFences[i]);
                    }
                }

                m_imageAvailableSemaphores.clear();
                m_renderFinishedSemaphores.clear();
                m_inFlightFences.clear();
                m_imagesInFlight.clear();
            }
        }

        uint32_t VK_Swapchain::getCurrentImageIndex() const {
            return m_currentImageIndex;
        }

        void VK_Swapchain::present() {
            auto dc = m_ctx->getDevice();

            try {
                vk::PresentInfoKHR presentInfo{};
                presentInfo.setWaitSemaphoreCount(1)
                        .setPWaitSemaphores(&m_renderFinishedSemaphores[m_currentFrame])
                        .setSwapchainCount(1)
                        .setPSwapchains(&m_swapchain)
                        .setPImageIndices(&m_currentImageIndex);

                auto result = m_presentQueue.presentKHR(presentInfo);

                m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

                dc.waitForFences(1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

                dc.resetFences(1, &m_inFlightFences[m_currentFrame]);

                auto nextResult = dc.acquireNextImageKHR(
                        m_swapchain,
                        UINT64_MAX,
                        m_imageAvailableSemaphores[m_currentFrame],
                        nullptr,
                        &m_currentImageIndex
                );

                m_fctImage->changeCurrentIndex(m_currentImageIndex);

                if (m_imagesInFlight[m_currentImageIndex]) {
                    dc.waitForFences(1, m_imagesInFlight[m_currentImageIndex], VK_TRUE, UINT64_MAX);
                }

                m_imagesInFlight[m_currentImageIndex] = &m_inFlightFences[m_currentFrame];

            } catch (const std::exception& e) {
                FCT::ferr << "Failed to present image: " << e.what() << std::endl;
                throw;
            }
        }
        void VK_Swapchain::acquireFirstImage() {
            auto dc = m_ctx->getDevice();

            try {
                dc.acquireNextImageKHR(
                        m_swapchain,
                        UINT64_MAX,
                        m_imageAvailableSemaphores[m_currentFrame],
                        nullptr,
                        &m_currentImageIndex
                );
                m_fctImage->changeCurrentIndex(m_currentImageIndex);

                if (m_imagesInFlight[m_currentImageIndex]) {
                    dc.waitForFences(1, m_imagesInFlight[m_currentImageIndex], VK_TRUE, UINT64_MAX);
                }

                m_imagesInFlight[m_currentImageIndex] = &m_inFlightFences[m_currentFrame];

            } catch (const std::exception& e) {
                FCT::ferr << "Failed to acquire first image: " << e.what() << std::endl;
                throw;
            }
        }

        vk::Semaphore VK_Swapchain::getImageAvailableSemaphore() const {
            return m_imageAvailableSemaphores[m_currentFrame];
        }

        vk::Semaphore VK_Swapchain::getRenderFinishedSemaphore() const {
            return m_renderFinishedSemaphores[m_currentFrame];
        }

        vk::Fence VK_Swapchain::getInFlightFence() const {
            return m_inFlightFences[m_currentFrame];
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
    }
}

