//
// Created by Administrator on 2025/3/1.
//
#include "./Context.h"
#ifndef VK_CONTEXT_H
#define VK_CONTEXT_H

namespace FCT {
    class VK_Context : public Context {
    public:
        VK_Context();
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

    private:

    };
}

#endif //VK_CONTEXT_H
