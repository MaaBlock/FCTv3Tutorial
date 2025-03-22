//
// Created by Administrator on 2025/3/4.
//
#include "../ThirdParty.h"
#include "./VK_Context.h"
#ifndef FCT_VK_SWAPCHAIN_H
#define FCT_VK_SWAPCHAIN_H
namespace FCT{
    class VK_Swapchain {
    public:
        VK_Swapchain(VK_Context* ctx);
        ~VK_Swapchain();
        void create(vk::SurfaceKHR surface, uint32_t width, uint32_t height);

        void destroy();
        void createImageViews();
        void destoryImageViews();

        uint32_t getCurrentImageIndex() const;
        void present();
        void acquireFirstImage();
        vk::Semaphore getImageAvailableSemaphore() const;
        vk::Semaphore getRenderFinishedSemaphore() const;
        vk::Fence getInFlightFence() const;
        vk::Extent2D getExtent() const;
    private:
        vk::Format m_imageFormat;
        vk::Queue m_presentQueue;
        uint32_t m_presentQueueFamilyIndex;
        VK_Context* m_ctx;
        vk::SurfaceKHR m_surface;
        vk::SwapchainKHR m_swapchain;
        vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
        vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
        std::vector<vk::Image> m_images;
        std::vector<vk::ImageView> m_imageViews;

        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        std::vector<vk::Semaphore> m_imageAvailableSemaphores;
        std::vector<vk::Semaphore> m_renderFinishedSemaphores;
        std::vector<vk::Fence> m_inFlightFences;
        std::vector<vk::Fence*> m_imagesInFlight;
        size_t m_currentFrame = 0;

        void createSyncObjects();

        void destroySyncObjects();

        vk::Extent2D m_extent;
        uint32_t m_currentImageIndex = 0;
    };
}

#endif //FCT_VK_SWAPCHAIN_H