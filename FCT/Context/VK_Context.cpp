//
// Created by Administrator on 2025/3/1.
//

#include "../FCTAPI.h"
#include "VK_Context.h"
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
namespace FCT {
    FCT::TextureArray *FCT::VK_Context::createTextureArray() {
        return nullptr;
    }
    void FCT::VK_Context::clear(float r, float g, float b) {

    }

    void VK_Context::viewport(int x, int y, int width, int height) {

    }

    VertexBuffer *VK_Context::createVertexBuffer(VertexArray *array) {
        return nullptr;
    }

    VertexShader *VK_Context::createVertexShader(VertexFactory *factory) {
        return nullptr;
    }

    PixelShader *VK_Context::createPixelShader(const ShaderOutput &output) {
        return nullptr;
    }

    Material *VK_Context::createMaterial(VertexShader *vertexShader, PixelShader *pixelShader) {
        return nullptr;
    }

    InputLayout *VK_Context::createInputLayout(VertexFactory *factory) {
        return nullptr;
    }

    DrawCall *VK_Context::createDrawCall(PrimitiveType primitiveType, uint32_t startVertex,
                                         uint32_t vertexCount) {
        return nullptr;
    }

    ConstBuffer *VK_Context::createConstBuffer() {
        return nullptr;
    }

    Texture *VK_Context::createTexture() {
        return nullptr;
    }


    VK_Context::~VK_Context() {

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

    void VK_Context::create(IRenderTarget *target) {
        target->bind(this);
    }



    Image *VK_Context::createImage() {
        return nullptr;
    }

    vk::Instance VK_Context::getVkInstance() {
        return m_common->getInstance();
    }

    void VK_Context::compilePasses() {

    }

    void VK_Context::submitPasses() {

    }
}
