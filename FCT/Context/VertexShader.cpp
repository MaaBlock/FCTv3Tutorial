//
// Created by Administrator on 2025/4/6.
//
#include "../FCTAPI.h"
#include "./ShaderGenerate.h"
//todo
       //1.从VertexLayout生成ShaderIn结构体
       //2.从PixelLayout生成ShaderOut结构体
       //3.从PixelLayout检测到Pos，然后自动转换为SV_Position输出
       //4.生成下列系统值
       /*VertexIndex
        *InstanceIndex
        *PrimitiveID
        *ViewportIndex//ignore
        */
//todo:
        //1.从PixelLayout生成ShaderIn结构体
        //2用系统值填充ShaderOut结构体


namespace FCT
{
    void VertexShader::generateDefaultCode()
    {
        m_userSource = generatDefaultUserVertexMain(m_vertexLayouts,m_pixelLayout);
    }

    void VertexShader::preprocess()
    {
        std::map<std::string,uint32_t> locations;
        m_source = generatVertexShader(m_vertexLayouts, m_pixelLayout,locations,m_userSource);
        m_binaryCode.location(locations);
    }
}


