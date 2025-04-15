//
// Created by Administrator on 2025/3/1.
//

#include "../FCTAPI.h"
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
namespace FCT {
    FCT::TextureArray *FCT::VK_Context::createTextureArray() {
        return nullptr;
    }
    void FCT::VK_Context::clear(float r, float g, float b) {

    }

    void VK_Context::viewport(int x, int y, int width, int height) {

    }

    RHI::DescriptorPool* VK_Context::createDescriptorPool()
    {
        return new RHI::VK_DescriptorPool(this);
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


    VK_Context::~VK_Context() {
        if (m_device) {
            m_device.freeCommandBuffers(m_commandPool, m_commandBuffers);
            m_device.destroyCommandPool(m_commandPool);
            m_device.destroy();
        }
    }

    VK_Context::VK_Context(VK_ContextCommon *common) {
        m_common = common;
        m_phyDevice = common->getPhysicalDevice();
        auto queueFamily = m_phyDevice.getQueueFamilyProperties();

        for (size_t i = 0; i < queueFamily.size(); ++i) {
            if (queueFamily[i].queueFlags & (vk::QueueFlagBits::eGraphics)) {
                m_graphicsQueueFamilyIndex = i;
                break;
            }
        }
        vk::DeviceCreateInfo deviceCreateInfo;
        vk::DeviceQueueCreateInfo queueCreateInfo;
        float queuePriority = 1.0f;

        queueCreateInfo.setQueueFamilyIndex(m_graphicsQueueFamilyIndex)
                       .setQueueCount(1)
                       .setQueuePriorities(queuePriority);

        std::vector<const char*> deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        deviceCreateInfo.setQueueCreateInfos(queueCreateInfo)
                        .setPEnabledExtensionNames(deviceExtensions);

        m_device = m_phyDevice.createDevice(deviceCreateInfo);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_device);

        m_graphicsQueue = m_device.getQueue(m_graphicsQueueFamilyIndex, 0);


    }
    uint32_t VK_Context::getGraphicsQueueFamily() const {
        return m_graphicsQueueFamilyIndex;
    }

    void VK_Context::createCommandPoolAndBuffers() {
        vk::CommandPoolCreateInfo poolInfo;
        poolInfo.setQueueFamilyIndex(m_graphicsQueueFamilyIndex)
                 .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        m_commandPool = m_device.createCommandPool(poolInfo);

        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.setCommandPool(m_commandPool)
                  .setLevel(vk::CommandBufferLevel::ePrimary)
                  .setCommandBufferCount(1);

        m_commandBuffers = m_device.allocateCommandBuffers(allocInfo);
    }

   /* void VK_Context::create(IRenderTarget *target) {
        //target->bind(this);
    }*/

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

    void VK_Context::compilePasses() {

    }

    void VK_Context::submitPasses() {

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
}
