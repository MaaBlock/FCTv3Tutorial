//
// Created by Administrator on 2025/4/6.
//
#include "./VertexShader.h"
#ifndef VK_VERTEXSHADER_H
#define VK_VERTEXSHADER_H
namespace FCT
{
    class VK_VertexShader : public VertexShader{
    public:
        VK_VertexShader(VK_Context* ctx);
    private:
        VK_Context* m_ctx;
    };
}

#endif //VK_VERTEXSHADER_H
