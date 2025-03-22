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

        VertexShader *createVertexShader(VertexFactory *factory) override;

        PixelShader *createPixelShader(const ShaderOutput &output) override;

        Material *createMaterial(VertexShader *vertexShader, PixelShader *pixelShader) override;

        InputLayout *createInputLayout(VertexFactory *factory) override;

        DrawCall *createDrawCall(PrimitiveType primitiveType, uint32_t startVertex,
                                 uint32_t vertexCount) override;

        ConstBuffer *createConstBuffer() override;

        Texture *createTexture() override;
        Image *createImage() override;
        void create(IRenderTarget* target) override;
        void create(){

        }
        auto getDevice() const {
            return m_device;
        }
        auto getPhysicalDevice() const {
            return m_phyDevice;
        }
        vk::Instance getVkInstance();

        void compilePasses();
        void submitPasses();
        void clear(Vec4 color,float depth = 1.0,float stencil = 0.0){

        }
        uint32_t getGraphicsQueueFamily() const;
    private:
        VK_ContextCommon* m_common;
        size_t m_graphicsQueueFamilyIndex;
        vk::Device m_device;
        vk::PhysicalDevice m_phyDevice;
        vk::Queue m_graphicsQueue;

    };
}

#endif //VK_CONTEXT_H
