//
// Created by Administrator on 2025/3/1.
//

#include "../FCTAPI.h"
#include "VK_Context.h"

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

    VK_Context::VK_Context() {

    }
}
