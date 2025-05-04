//
// Created by Administrator on 2025/3/1.
//

#include "../FCTAPI.h"
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
namespace FCT
{
    FCT::TextureArray *FCT::VK_Context::createTextureArray() {
        return nullptr;
    }

    Sampler* VK_Context::createSampler()
    {
        return FCT_NEW(RHI::VK_Sampler,this);
    }

    void FCT::VK_Context::clear(float r, float g, float b) {

    }

    void VK_Context::viewport(int x, int y, int width, int height) {

    }

    RHI::DescriptorPool* VK_Context::createDescriptorPool()
    {
        return FCT_NEW(RHI::VK_DescriptorPool,this);
    }

    VertexShader* VK_Context::createVertexShader()
    {
        return new VK_VertexShader(this);
    }

    RHI::VertexShader* VK_Context::newRhiVertexShader()
    {
        return new RHI::VK_VertexShader(this);
    }

    RHI::PixelShader* VK_Context::newRhiPixelShader()
    {
        return new RHI::VK_PixelShader(this);
    }
    PixelShader* VK_Context::createPixelShader()
    {
        return new PixelShader(this);
    }

    RHI::InputLayout* VK_Context::createInputLayout()
    {
        return new RHI::VK_InputLayout(this);
    }

    /*Material *VK_Context::createMaterial(VertexShader *vertexShader, PixelShader *pixelShader) {
        return nullptr;
    }*/

    DrawCall *VK_Context::createDrawCall(PrimitiveType primitiveType, uint32_t startVertex,
                                         uint32_t vertexCount) {
        return nullptr;
    }

    RHI::TextureView* VK_Context::createTextureView()
    {
        return new RHI::VK_TextureView(this);
    }

    RHI::Image* VK_Context::newRhiImage()
    {
        return new RHI::VK_Image(this);
    }

    RHI::ConstBuffer *VK_Context::createConstBuffer() {
        return new RHI::VK_ConstBuffer(this);
    }

    RHI::RasterizationPipeline* VK_Context::createTraditionPipeline()
    {
        return new RHI::VK_TraditionalPipeline(this);
    }

    Texture *VK_Context::createTexture() {
        return nullptr;
    }

    VK_Context::~VK_Context()
    {
        if (m_device) {
            m_device.freeCommandBuffers(m_commandPool, m_commandBuffers);
            m_device.destroyCommandPool(m_commandPool);
            if (m_transferCommandPool) {
                m_device.destroyCommandPool(m_transferCommandPool);
            }
            m_device.destroy();
        }
    }

    VK_Context::VK_Context(VK_ContextCommon *common) : Context(common->runtime()) {
        m_common = common;
        m_phyDevice = common->getPhysicalDevice();
        auto queueFamily = m_phyDevice.getQueueFamilyProperties();

        m_transferQueueFamilyIndex = UINT32_MAX;
        for (size_t i = 0; i < queueFamily.size(); ++i) {
            if (queueFamily[i].queueFlags & vk::QueueFlagBits::eGraphics) {
                m_graphicsQueueFamilyIndex = i;
            }

            if (queueFamily[i].queueFlags & vk::QueueFlagBits::eTransfer) {
                if (!(queueFamily[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
                    !(queueFamily[i].queueFlags & vk::QueueFlagBits::eCompute)) {
                    m_transferQueueFamilyIndex = i;
                }
                else if (m_transferQueueFamilyIndex == UINT32_MAX) {
                    m_transferQueueFamilyIndex = i;
                }
            }
        }

        if (m_transferQueueFamilyIndex == UINT32_MAX) {
            m_transferQueueFamilyIndex = m_graphicsQueueFamilyIndex;
        }

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {m_graphicsQueueFamilyIndex, m_transferQueueFamilyIndex};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            vk::DeviceQueueCreateInfo queueCreateInfo;
            queueCreateInfo.setQueueFamilyIndex(queueFamily)
                           .setQueueCount(1)
                           .setPQueuePriorities(&queuePriority);
            queueCreateInfos.push_back(queueCreateInfo);
        }

        std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        vk::PhysicalDeviceFeatures deviceFeatures;
        deviceFeatures.setSamplerAnisotropy(true);

        vk::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.setQueueCreateInfos(queueCreateInfos)
                        .setPEnabledExtensionNames(deviceExtensions)
                        .setPEnabledFeatures(&deviceFeatures);

        m_device = m_phyDevice.createDevice(deviceCreateInfo);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_device);

        m_graphicsQueue = m_device.getQueue(m_graphicsQueueFamilyIndex, 0);
        m_transferQueue = m_device.getQueue(m_transferQueueFamilyIndex, 0);

        createCommandPoolAndBuffers();
        createTransferCommandPool();
    }
    uint32_t VK_Context::getGraphicsQueueFamily() const {
        return m_graphicsQueueFamilyIndex;
    }
    void VK_Context::createTransferCommandPool() {
        vk::CommandPoolCreateInfo poolInfo;
        poolInfo.setQueueFamilyIndex(m_transferQueueFamilyIndex)
                .setFlags(vk::CommandPoolCreateFlagBits::eTransient |
                          vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        m_transferCommandPool = m_device.createCommandPool(poolInfo);
    }
    vk::CommandBuffer VK_Context::beginSingleTimeTransferCommands() {
        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
        allocInfo.setCommandPool(m_transferCommandPool);
        allocInfo.setCommandBufferCount(1);

        vk::CommandBuffer commandBuffer = m_device.allocateCommandBuffers(allocInfo)[0];

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        commandBuffer.begin(beginInfo);

        return commandBuffer;
    }

    void VK_Context::endSingleTimeTransferCommands(vk::CommandBuffer commandBuffer, vk::Fence* outFence) {
        commandBuffer.end();

        vk::FenceCreateInfo fenceInfo;
        vk::Fence fence = m_device.createFence(fenceInfo);

        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(&commandBuffer);

        m_transferQueue.submit(1, &submitInfo, fence);

        if (outFence) {
            *outFence = fence;
        } else {
            m_device.waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
            m_device.destroyFence(fence);
        m_device.freeCommandBuffers(m_transferCommandPool, 1, &commandBuffer);
        }

    }

    vk::CommandBuffer VK_Context::beginSingleTimeCommands()
    {
        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
        allocInfo.setCommandPool(m_commandPool);
        allocInfo.setCommandBufferCount(1);

        vk::CommandBuffer commandBuffer = m_device.allocateCommandBuffers(allocInfo)[0];

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        commandBuffer.begin(beginInfo);

        return commandBuffer;
    }

    void VK_Context::endSingleTimeCommands(vk::CommandBuffer commandBuffer)
    {
        commandBuffer.end();

        vk::FenceCreateInfo fenceInfo;
        vk::Fence fence = m_device.createFence(fenceInfo);

        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBufferCount(1);
        submitInfo.setPCommandBuffers(&commandBuffer);

        m_graphicsQueue.submit(1, &submitInfo, fence);
        m_device.waitForFences(1, &fence, VK_TRUE, UINT64_MAX);
        m_device.destroyFence(fence);

        m_device.freeCommandBuffers(m_commandPool, 1, &commandBuffer);
    }


    void VK_Context::createCommandPoolAndBuffers() {
        vk::CommandPoolCreateInfo poolInfo;
        poolInfo.setQueueFamilyIndex(m_graphicsQueueFamilyIndex)
                 .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        m_commandPool = m_device.createCommandPool(poolInfo);
/*
        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.setCommandPool(m_commandPool)
                  .setLevel(vk::CommandBufferLevel::ePrimary)
                  .setCommandBufferCount(1);

        m_commandBuffers = m_device.allocateCommandBuffers(allocInfo);*/
    }

    /* void VK_Context::create(IRenderTarget *target) {
         //target->bind(this);
     }*/

    void VK_Context::transferDataToBuffer(vk::Buffer dstBuffer, size_t size, const void* data)
    {
        vk::BufferCreateInfo stagingBufferInfo;
        stagingBufferInfo.setSize(size);
        stagingBufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc);
        stagingBufferInfo.setSharingMode(vk::SharingMode::eExclusive);

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingMemory;

        try {
            stagingBuffer = m_device.createBuffer(stagingBufferInfo);

            vk::MemoryRequirements memRequirements = m_device.getBufferMemoryRequirements(stagingBuffer);

            vk::MemoryAllocateInfo allocInfo;
            allocInfo.setAllocationSize(memRequirements.size);
            allocInfo.setMemoryTypeIndex(findMemoryType(
                memRequirements.memoryTypeBits,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

            stagingMemory = m_device.allocateMemory(allocInfo);
            m_device.bindBufferMemory(stagingBuffer, stagingMemory, 0);

            void* mappedData = m_device.mapMemory(stagingMemory, 0, size);
            memcpy(mappedData, data, size);
            m_device.unmapMemory(stagingMemory);

            vk::CommandBuffer cmdBuffer = beginSingleTimeCommands();

            vk::BufferCopy copyRegion;
            copyRegion.setSrcOffset(0);
            copyRegion.setDstOffset(0);
            copyRegion.setSize(size);
            cmdBuffer.copyBuffer(stagingBuffer, dstBuffer, 1, &copyRegion);

            endSingleTimeCommands(cmdBuffer);

            m_device.destroyBuffer(stagingBuffer);
            m_device.freeMemory(stagingMemory);
        }
        catch (const std::exception& e) {
            ferr << "Failed to transfer data to buffer: " << e.what() << std::endl;

            if (stagingBuffer) {
                m_device.destroyBuffer(stagingBuffer);
            }

            if (stagingMemory) {
                m_device.freeMemory(stagingMemory);
            }

            throw;
        }
    }
    void VK_Context::transferDataToImage(vk::Image dstImage, uint32_t width, uint32_t height,
                                        vk::Format format, const void* data, size_t dataSize, vk::Fence* outFence, std::function<void()>* cleanUpCallback)
    {
        transferDataToImage(dstImage, width, height, 1, format, 1, 1,
                           vk::ImageAspectFlagBits::eColor, data, dataSize, outFence, cleanUpCallback);
    }

   void VK_Context::transferDataToImage(vk::Image dstImage, uint32_t width, uint32_t height, uint32_t depth,
                                vk::Format format, uint32_t mipLevels, uint32_t arrayLayers,
                                vk::ImageAspectFlags aspectMask, const void* data, size_t dataSize,
                                vk::Fence* outFence, std::function<void()>* cleanUpCallback)
    {
        size_t pixelSize = FormatSize(FromVkFormat(format));
        size_t fullImageSize = width * height * depth * pixelSize;
        vk::BufferCreateInfo stagingBufferInfo;
        stagingBufferInfo.setSize(fullImageSize);
        stagingBufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc);
        stagingBufferInfo.setSharingMode(vk::SharingMode::eExclusive);

        vk::Buffer stagingBuffer;
        vk::DeviceMemory stagingMemory;

        try {
            stagingBuffer = m_device.createBuffer(stagingBufferInfo);

            vk::MemoryRequirements memRequirements = m_device.getBufferMemoryRequirements(stagingBuffer);

            vk::MemoryAllocateInfo allocInfo;
            allocInfo.setAllocationSize(memRequirements.size);
            allocInfo.setMemoryTypeIndex(findMemoryType(
                memRequirements.memoryTypeBits,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

            stagingMemory = m_device.allocateMemory(allocInfo);
            m_device.bindBufferMemory(stagingBuffer, stagingMemory, 0);

            void* mappedData = m_device.mapMemory(stagingMemory, 0, dataSize);
            memcpy(mappedData, data, dataSize);
            m_device.unmapMemory(stagingMemory);
            vk::CommandBuffer cmdBuffer = beginSingleTimeTransferCommands();

            bool needsQueueTransfer = m_graphicsQueueFamilyIndex != m_transferQueueFamilyIndex;

            vk::ImageLayout finalLayout;
            vk::AccessFlags finalAccessMask;

            if (aspectMask & vk::ImageAspectFlagBits::eDepth || aspectMask & vk::ImageAspectFlagBits::eStencil) {
                finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
                finalAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            } else {
                finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                finalAccessMask = vk::AccessFlagBits::eShaderRead;
            }

            vk::ImageMemoryBarrier barrier;
            barrier.setOldLayout(vk::ImageLayout::eUndefined);
            barrier.setNewLayout(vk::ImageLayout::eTransferDstOptimal);

            if (needsQueueTransfer) {
                barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
                barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
            } else {
                barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
                barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
            }

            barrier.setImage(dstImage);
            barrier.setSubresourceRange(vk::ImageSubresourceRange(
                aspectMask, 0, mipLevels, 0, arrayLayers));
            barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
            barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

            cmdBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTopOfPipe,
                vk::PipelineStageFlagBits::eTransfer,
                vk::DependencyFlags(),
                0, nullptr,
                0, nullptr,
                1, &barrier);

            vk::BufferImageCopy region;
            region.setBufferOffset(0);
            region.setBufferRowLength(0);
            region.setBufferImageHeight(0);
            region.setImageSubresource(vk::ImageSubresourceLayers(
                aspectMask, 0, 0, arrayLayers));
            region.setImageOffset(vk::Offset3D(0, 0, 0));
            region.setImageExtent(vk::Extent3D(width, height, depth));

            cmdBuffer.copyBufferToImage(
                stagingBuffer,
                dstImage,
                vk::ImageLayout::eTransferDstOptimal,
                1, &region);

            barrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);

            if (needsQueueTransfer) {
                barrier.setNewLayout(vk::ImageLayout::eTransferDstOptimal);
                barrier.setSrcQueueFamilyIndex(m_transferQueueFamilyIndex);
                barrier.setDstQueueFamilyIndex(m_graphicsQueueFamilyIndex);
                barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
                barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);
            } else {
                barrier.setNewLayout(finalLayout);
                barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
                barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
                barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
                barrier.setDstAccessMask(finalAccessMask);
            }

            cmdBuffer.pipelineBarrier(
                vk::PipelineStageFlagBits::eTransfer,
                vk::PipelineStageFlagBits::eTransfer,
                vk::DependencyFlags(),
                0, nullptr,
                0, nullptr,
                1, &barrier);

            if (outFence) {
                vk::Fence fence;
                endSingleTimeTransferCommands(cmdBuffer, &fence);

                if (cleanUpCallback) {
                    vk::Buffer capturedBuffer = stagingBuffer;
                    vk::DeviceMemory capturedMemory = stagingMemory;
                    vk::Image capturedImage = dstImage;
                    bool capturedNeedsQueueTransfer = needsQueueTransfer;
                    vk::ImageAspectFlags capturedAspectMask = aspectMask;
                    uint32_t capturedMipLevels = mipLevels;
                    uint32_t capturedArrayLayers = arrayLayers;
                    vk::ImageLayout capturedFinalLayout = finalLayout;
                    vk::AccessFlags capturedFinalAccessMask = finalAccessMask;
                    vk::Fence capturedFence = fence;
                    vk::CommandBuffer capturedCmdBuffer = cmdBuffer;

                    *cleanUpCallback = [this, capturedBuffer, capturedMemory, capturedImage,
                                       capturedNeedsQueueTransfer, capturedAspectMask,
                                       capturedMipLevels, capturedArrayLayers,
                                       capturedFinalLayout, capturedFinalAccessMask,
                                       capturedFence,capturedCmdBuffer]() {
                        m_device.waitForFences(1, &capturedFence, VK_TRUE, UINT64_MAX);
                        m_device.destroyFence(capturedFence);
                        postLogicTask([this,capturedCmdBuffer]()
                        {
                            m_device.freeCommandBuffers(m_transferCommandPool, 1, &capturedCmdBuffer);
                        });
                        if (capturedNeedsQueueTransfer) {
                            vk::CommandBuffer graphicsCmdBuffer = beginSingleTimeCommands();

                            vk::ImageMemoryBarrier finalBarrier;
                            finalBarrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
                            finalBarrier.setNewLayout(capturedFinalLayout);
                            finalBarrier.setSrcQueueFamilyIndex(m_transferQueueFamilyIndex);
                            finalBarrier.setDstQueueFamilyIndex(m_graphicsQueueFamilyIndex);
                            finalBarrier.setImage(capturedImage);
                            finalBarrier.setSubresourceRange(vk::ImageSubresourceRange(
                                capturedAspectMask, 0, capturedMipLevels, 0, capturedArrayLayers));
                            finalBarrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
                            finalBarrier.setDstAccessMask(capturedFinalAccessMask);

                            vk::PipelineStageFlags finalStage;
                            if (capturedAspectMask & vk::ImageAspectFlagBits::eDepth ||
                                capturedAspectMask & vk::ImageAspectFlagBits::eStencil) {
                                finalStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
                                } else {
                                    finalStage = vk::PipelineStageFlagBits::eAllCommands;
                                }

                            graphicsCmdBuffer.pipelineBarrier(
                                vk::PipelineStageFlagBits::eAllCommands,
                                finalStage,
                                vk::DependencyFlags(),
                                0, nullptr,
                                0, nullptr,
                                1, &finalBarrier);

                            endSingleTimeCommands(graphicsCmdBuffer);
                        }

                        if (capturedBuffer) {
                            m_device.destroyBuffer(capturedBuffer);
                        }
                        if (capturedMemory) {
                            m_device.freeMemory(capturedMemory);
                        }
                    };

                    if (outFence) {
                        *outFence = fence;
                    }
                } else {
                    ferr << "callback is nullptr, resource is leaked in transferDataToImage" << std::endl;
                }
            } else {
                endSingleTimeTransferCommands(cmdBuffer, nullptr);

                if (needsQueueTransfer) {
                    vk::CommandBuffer graphicsCmdBuffer = beginSingleTimeCommands();

                    vk::ImageMemoryBarrier finalBarrier;
                    finalBarrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal);
                    finalBarrier.setNewLayout(finalLayout);
                    finalBarrier.setSrcQueueFamilyIndex(m_transferQueueFamilyIndex);
                    finalBarrier.setDstQueueFamilyIndex(m_graphicsQueueFamilyIndex);
                    finalBarrier.setImage(dstImage);
                    finalBarrier.setSubresourceRange(vk::ImageSubresourceRange(
                        aspectMask, 0, mipLevels, 0, arrayLayers));
                    finalBarrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
                    finalBarrier.setDstAccessMask(finalAccessMask);

                    vk::PipelineStageFlags finalStage;
                    if (aspectMask & vk::ImageAspectFlagBits::eDepth ||
                        aspectMask & vk::ImageAspectFlagBits::eStencil) {
                        finalStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
                        } else {
                            finalStage = vk::PipelineStageFlagBits::eAllCommands;
                        }

                    graphicsCmdBuffer.pipelineBarrier(
                        vk::PipelineStageFlagBits::eAllCommands,
                        finalStage,
                        vk::DependencyFlags(),
                        0, nullptr,
                        0, nullptr,
                        1, &finalBarrier);

                    endSingleTimeCommands(graphicsCmdBuffer);
                }

                m_device.destroyBuffer(stagingBuffer);
                m_device.freeMemory(stagingMemory);
            }
        }
        catch (const std::exception& e) {
            ferr << "Failed to transfer data to image: " << e.what() << std::endl;

            if (stagingBuffer) {
                m_device.destroyBuffer(stagingBuffer);
            }

            if (stagingMemory) {
                m_device.freeMemory(stagingMemory);
            }

            throw;
        }
    }

    void VK_Context::create()
    {

    }


    Image *VK_Context::createImage() {
        return nullptr;
    }

    RHI::Swapchain* VK_Context::createSwapchain()
    {
        return new RHI::VK_Swapchain(this);
    }

    RHI::RenderTargetView* VK_Context::createRenderTargetView()
    {
        return new RHI::VK_RenderTargetView(this);
    }

    RHI::Pass* VK_Context::createPass()
    {
        return new RHI::VK_Pass(this);
    }

    RHI::PassGroup* VK_Context::createPassGroup()
    {
        return new RHI::VK_PassGroup(this);
    }

    RHI::Semaphore* VK_Context::createSemaphore()
    {
        return new RHI::VK_Semaphore(this);
    }

    RHI::Fence* VK_Context::createFence()
    {
        return new RHI::VK_Fence(this);
    }

    RHI::VertexBuffer* VK_Context::createVertexBuffer()
    {
        return new RHI::VK_VertexBuffer(this);
    }

    RHI::IndexBuffer* VK_Context::createIndexBuffer()
    {
        return new RHI::VK_IndexBuffer(this);
    }

    PassResource* VK_Context::createPassResource()
    {
        return new VK_PassResource(this);
    }

    uint32_t VK_Context::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
    {
        vk::PhysicalDeviceMemoryProperties memProperties = getPhysicalDevice().getMemoryProperties();

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
                }
        }
    }

    RHI::CommandPool* VK_Context::createCommandPool()
    {
        return new RHI::VK_CommandPool(this);
    }

    vk::Instance VK_Context::getVkInstance() {
        return m_common->getInstance();
    }

    void VK_Context::beginCommandBuffer(int index) {
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

        m_commandBuffers[index].begin(beginInfo);
    }

    void VK_Context::endCommandBuffer(int index) {
        m_commandBuffers[index].end();
    }
    void VK_Context::submitCommandBuffer() {
        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(m_commandBuffers[0]);

        m_graphicsQueue.submit(submitInfo);
        m_graphicsQueue.waitIdle();
    }

    RHI::DepthStencilView* VK_Context::createDepthStencilView()
    {
        return new RHI::VK_DepthStencilView(this);
    }
}
