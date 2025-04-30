//
// Created by Administrator on 2025/4/7.
//
#include "../ThirdParty.h"
#include "./Vertex.h"
#include "Uniform.h"
#ifndef SHADERGENERATE_H
#define SHADERGENERATE_H
/*
 *
 *对于PixelLayout
 *使用element的序号来当vk的location
 *直接使用语义来当 dx12的语义
 *变量名为 语义
 */
namespace FCT
{
    enum class ShaderKind
    {
        VertexShader,
        FragmentShader,
        ComputeShader,
        GeometryShader,
        TessControlShader,
        MeshShader,
      };
}
#endif //SHADERGENERATE_H
