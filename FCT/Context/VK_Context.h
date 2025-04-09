//
// Created by Administrator on 2025/3/1.
//
#include "../ThirdParty.h"
#include "./Context.h"
#ifndef VK_CONTEXT_H
#define VK_CONTEXT_H

namespace FCT {
    class VK_ContextCommon;
    class VK_Context : public Context {
    public:
        VK_Context(VK_ContextCommon* common);
        ~VK_Context() override;
        TextureArray *createTextureArray() override;

        void clear(float r, float g, float b) override;

        void viewport(int x, int y, int width, int height) override;

        VertexBuffer *createVertexBuffer(VertexArray *array) override;
        VertexShader* createVertexShader() override;
        RHI::VertexShader* newRhiVertexShader() override;
        RHI::PixelShader* newRhiPixelShader() override;
        PixelShader* createPixelShader() override;
        RHI::InputLayout* createInputLayout() override;
        DrawCall *createDrawCall(PrimitiveType primitiveType, uint32_t startVertex,
                                 uint32_t vertexCount) override;

        ConstBuffer *createConstBuffer() override;
        RHI::RasterizationPipeline* createTraditionPipeline() override;
        Texture *createTexture() override;
        Image *createImage() override;
        RHI::Swapchain* createSwapchain() override;
        RHI::RenderTargetView* createRenderTargetView() override;
        RHI::Pass* createPass() override;
        RHI::PassGroup* createPassGroup() override;
        RHI::Semaphore* createSemaphore() override;
        RHI::Fence* createFence() override;
        RHI::VertexBuffer* createVertexBuffer() override;
        //void create(IRenderTarget* target) override;
        void create();
        auto device()
        {
            return m_device;
        }
        auto getDevice() const {
            return m_device;
        }
        auto getPhysicalDevice() const {
            return m_phyDevice;
        }
        RHI::CommandPool* createCommandPool() override;
        vk::Instance getVkInstance();
        void compilePasses() override;
        void submitPasses() override;
        void beginCommandBuffer(int index);
        void endCommandBuffer(int index);
        void submitCommandBuffer();

        void clear(Vec4 color,float depth = 1.0,float stencil = 0.0){

        }
        uint32_t getGraphicsQueueFamily() const;
        vk::Queue getGraphicsQueue()
        {
            return m_graphicsQueue;
        }
    private:
        void createCommandPoolAndBuffers();
        VK_ContextCommon* m_common;
        size_t m_graphicsQueueFamilyIndex;
        vk::Device m_device;
        vk::PhysicalDevice m_phyDevice;
        vk::Queue m_graphicsQueue;
        vk::CommandPool m_commandPool;
        std::vector<vk::CommandBuffer> m_commandBuffers;
    };
}

#endif //VK_CONTEXT_H
