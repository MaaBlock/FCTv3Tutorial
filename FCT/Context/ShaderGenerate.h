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

    //locationMap使用前自己clear
    inline std::string generatShaderIn(const std::map<uint32_t, VertexLayout>& layouts,std::map<std::string,uint32_t>& locationMap)
    {
        std::stringstream ss;
        ss << "//FCT Input Structure\n";
        ss << "struct ShaderIn {\n";

        // 生成系统值
        ss << "    uint vertexID : SV_VertexID;\n";
        ss << "    uint instanceID : SV_InstanceID;\n\n";

        // 检查语义 冲突
        std::set<std::string> usedSemantics;
        for (const auto& [slot, layout] : layouts) {
            for (size_t i = 0; i < layout.getElementCount(); i++) {
                const VertexElement& element = layout.getElement(i);
                const char* semantic = element.getSemantic();

                if (usedSemantics.find(semantic) != usedSemantics.end()) {
                    throw std::runtime_error(std::string("Semantic conflict detected: ") + semantic);
                }

                usedSemantics.insert(semantic);
            }
        }

        int nextLocation = 0;

        for (const auto& [slot, layout] : layouts) {
            ss << "    // Vertex buffer slot " << slot << "\n";

            for (size_t i = 0; i < layout.getElementCount(); i++) {
                const VertexElement& element = layout.getElement(i);
                std::string typeStr = FormatToShaderType(element.getFormat());
                const char* semantic = element.getSemantic();

                locationMap[semantic] = nextLocation;
                ss << "    [[vk::location(" << nextLocation << ")]] ";
                ss << typeStr << " " << semantic << " : " << semantic << ";\n";

                nextLocation++;
            }

            if (slot != layouts.rbegin()->first) {
                ss << "\n";
            }
        }

        ss << "};\n";
        return ss.str();
    }

    inline std::string generatShaderIn(VertexLayout layout)
    {
        std::stringstream ss;
        ss << "//FCT Input Structure\n";
        ss << "struct ShaderIn {\n";


        //生成系统值
        ss << "    uint vertexID : SV_VertexID;\n";
        ss << "    uint instanceID : SV_InstanceID;\n";


        //翻译VertexLayout
        for (size_t i = 0; i < layout.getElementCount(); i++) {
            const VertexElement& element = layout.getElement(i);

            std::string typeStr = FormatToShaderType(element.getFormat());

            const char* semantic = element.getSemantic();
            ss << "    [[vk::location(" << i << ")]] ";
            ss << typeStr << " " << semantic << " : " << semantic << ";\n";
        }


        ss << "};\n";
        return ss.str();
    }
    inline std::string generatVertexMain(std::map<uint32_t, VertexLayout> vertexLayouts, PixelLayout pixleLayout)
    {
        std::stringstream ss;
        ss << "ShaderOut main(ShaderIn);\n";
        ss << "ShaderOut FCTEntry(ShaderIn sIn)\n";
        ss << " {\n";
        ss << "    ShaderOut sOut;\n";
        ss << "    \n";

        bool hasPosition = false;
        const VertexElement* posElement = nullptr;

        posElement = pixleLayout.getElementByType(VtxType::Position3f);
        if (posElement) {
            hasPosition = true;
        }
        else if ((posElement = pixleLayout.getElementByType(VtxType::Position2f))) {
            hasPosition = true;
        }

        ss << "    sOut = main(sIn);\n";

        if (hasPosition && posElement) {
            const char* semantic = posElement->getSemantic();
            VtxType type = posElement->getType();

            if (type == VtxType::Position3f) {
                ss << "    sOut.outPosition = float4(sOut." << semantic << ", 1.0f);\n";
            } else if (type == VtxType::Position2f) {
                ss << "    sOut.outPosition = float4(sOut." << semantic << ", 0.0f, 1.0f);\n";
            }
        } else {
            ss << "    sOut.outPosition = float4(0.0f, 0.0f, 0.0f, 1.0f);\n";
        }

        ss << "    \n";
        ss << "    return sOut;\n";
        ss << "}\n";

        return ss.str();
    }
    inline std::string generatDefaultUserVertexMain(std::map<uint32_t, VertexLayout> vertexLayouts, PixelLayout pixelLayout)
    {
        std::stringstream ss;
        ss << "//FCT Default User Vertex Main\n";
        ss << "ShaderOut main(ShaderIn sIn) {\n";
        ss << "    ShaderOut sOut;\n";
        ss << "    \n";

        for (const auto& [slot, layout] : vertexLayouts) {
            for (size_t i = 0; i < layout.getElementCount(); i++) {
                const VertexElement& inElement = layout.getElement(i);
                const char* semantic = inElement.getSemantic();

                if (inElement.getType() != VtxType::Custom)
                {
                    auto element = pixelLayout.getElementByType(inElement.getType());
                    if (element)
                    {
                        ss << "    sOut." << element->getSemantic() << " = sIn." << semantic << ";\n";
                    }
                } else
                {
                    auto element = pixelLayout.getElementBySemantic(semantic);
                    if (element)
                    {
                        ss << "    sOut." << element->getSemantic() << " = sIn." << semantic << ";\n";
                    }
                    else
                    {
                        ss << "    //属性 " << semantic << " 在自动生成时被忽略 \n";
                    }
                }
            }
        }

        ss << "    \n";
        ss << "    return sOut;\n";
        ss << "}\n";

        return ss.str();
    }
    inline std::string generatFromPixelLayout(PixelLayout layout)
    {
        std::stringstream ss;
        ss << "    //PixelLayout\n";

        for (size_t i = 0; i < layout.getElementCount(); i++)
        {
            const VertexElement& element = layout.getElement(i);
            std::string typeStr = FormatToShaderType(element.getFormat());
            const char* semantic = element.getSemantic();
            ss << "    [[vk::location(" << i << ")]] ";
            ss << typeStr << " " << semantic << " : " << semantic << ";\n";
        }

        return ss.str();
    }

    inline std::string generatShaderIn(PixelLayout layout) //PS in
    {
        std::stringstream ss;
        ss << "//FCT Input Structure\n";
        ss << "struct ShaderIn {\n";

        ss << "    float4 position : SV_POSITION;\n";

        ss << generatFromPixelLayout(layout);

        ss << "};\n";
        return ss.str();
    }
    inline std::string generatShaderOut(PixelLayout layout) //顶点着色器out
    {
        std::stringstream ss;
        ss << "//FCT Output Structure\n";
        ss << "struct ShaderOut {\n";

        ss << "    float4 outPosition : SV_POSITION; \n";

        ss << generatFromPixelLayout(layout);

        ss << "};\n";
        return ss.str();
    }
    inline std::string generatShaderOut()
    {
        std::stringstream ss;
        ss << "//FCT Output Structure\n";
        ss << "struct ShaderOut {\n";
        ss << "    float4 target0 : SV_Target0;\n";
        ss << "    float4 target1 : SV_Target1;\n";
        ss << "    float4 target2 : SV_Target2;\n";
        ss << "    float4 target3 : SV_Target3;\n";
        ss << "    float4 target4 : SV_Target4;\n";
        ss << "    float4 target5 : SV_Target5;\n";
        ss << "    float4 target6 : SV_Target6;\n";
        ss << "    float4 target7 : SV_Target7;\n";
        ss << "};\n";
        return ss.str();
    }

    inline std::string uniformTypeToShaderType(UniformType type)
    {
        switch (type) {
        case UniformType::ModelMatrix:
        case UniformType::ViewMatrix:
        case UniformType::ProjectionMatrix:
        case UniformType::MVPMatrix:
        case UniformType::Mat4:
            return "float4x4";
        case UniformType::Mat3:
            return "float3x3";
        case UniformType::Vec4:
            return "float4";
        case UniformType::Vec3:
            return "float3";
        case UniformType::Vec2:
            return "float2";
        case UniformType::Float:
            return "float";
        case UniformType::Int:
            return "int";
        case UniformType::Bool:
            return "bool";
        case UniformType::Texture2D:
        case UniformType::TextureCube:
        case UniformType::Custom:
        default:
            return "float4";
        }
    }
    inline std::string generatConstBuffer(
    const std::vector<UniformLayout>& uniformLayouts,
    std::map<std::string, std::pair<uint32_t, uint32_t>>& m_uniformsLocations,
    std::map<std::string, std::pair<uint32_t, uint32_t>>& m_constBufferLocations)
    {
        std::stringstream ss;
        ss << "//FCT Constant Buffers\n";

        std::map<UpdateFrequency, std::vector<const UniformLayout*>> frequencyGroups;

        for (const auto& layout : uniformLayouts) {
            frequencyGroups[layout.getUpdateFrequency()].push_back(&layout);
        }

        uint32_t setIndex = 0;
        std::map<UpdateFrequency, uint32_t> frequencyToSet;

        const std::array<UpdateFrequency, 5> priorityOrder = {
            UpdateFrequency::Static,
            UpdateFrequency::PerFrame,
            UpdateFrequency::PerObject,
            UpdateFrequency::Dynamic,
            UpdateFrequency::Default
        };

        for (const auto& freq : priorityOrder) {
            if (frequencyGroups.find(freq) != frequencyGroups.end()) {
                frequencyToSet[freq] = setIndex++;
            }
        }

        uint32_t cbRegister = 0;

        for (const auto& [freq, set] : frequencyToSet) {
            const auto& layouts = frequencyGroups[freq];


            uint32_t bindingIndex = 0;
            for (const auto* layout : layouts) {
                //ss << "[[vk::set(" << set << "), vk::binding(" << bindingIndex << ")]]\n";
                ss << "[[vk::binding(" << bindingIndex << ", " << set << ")]]\n";
                ss << "cbuffer " << layout->getName() << " : register(b" << cbRegister << ") {\n";

                m_constBufferLocations[layout->getName()] = std::make_pair(set, bindingIndex);
                m_uniformsLocations[layout->getName()] = std::make_pair(set, bindingIndex);

                for (size_t i = 0; i < layout->getElementCount(); ++i) {
                    const auto& element = layout->getElement(i);
                    std::string glslType = uniformTypeToShaderType(element.getType());
                    ss << "    " << glslType << " " << element.getName() << ";\n";
                }

                ss << "};\n\n";

                bindingIndex++;
                cbRegister++;
            }
        }

        return ss.str();
    }
    inline std::string generatVertexShader(
        std::map<uint32_t,VertexLayout> vertexLayouts,
        PixelLayout pixelLayout,
        std::vector<UniformLayout> uniformLayouts,
        RHI::ShaderBinary& binary,
        std::string userCode)
    {
        std::map<std::string,uint32_t> locations;
        std::map<std::string,std::pair<uint32_t,uint32_t>> uniformsLocations;
        std::map<std::string,std::pair<uint32_t,uint32_t>> constBufferLocations;
        std::stringstream ss;
        ss << generatShaderIn(vertexLayouts, locations);
        ss << generatShaderOut(pixelLayout);
        ss << generatConstBuffer(uniformLayouts, uniformsLocations, constBufferLocations);
        ss << generatVertexMain(vertexLayouts, pixelLayout);
        ss << userCode;
        binary.location(locations);
        binary.uniformLocation(uniformsLocations);
        binary.constBufferLocation(constBufferLocations);
        return ss.str();
    }


    inline std::string generatPixelMain(PixelLayout pixleLayout)
    {
        std::stringstream ss;
        ss << "ShaderOut main(ShaderIn sIn);\n";
        ss << "ShaderOut FCTEntry(ShaderIn sIn) {\n";
        ss << "    ShaderOut sOut;\n";
        ss << "    \n";
        ss << "    sOut = main(sIn);\n";
        ss << "    \n";
        ss << "    return sOut;\n";
        ss << "}\n";
        return ss.str();
    }

    inline std::string generatDefaultUserPixelMain(PixelLayout pixelLayout)
    {
        std::stringstream ss;
        ss << "//FCT Default User Pixel Main\n";
        ss << "ShaderOut main(ShaderIn sIn) {\n";
        ss << "    ShaderOut sOut;\n";
        auto colorElement = pixelLayout.getElementByType(VtxType::Color4f);
        if (!colorElement) {
            colorElement = pixelLayout.getElementByType(VtxType::Color3f);
        }
        if (colorElement) {
            const char* semantic = colorElement->getSemantic();
            VtxType type = colorElement->getType();

            if (type == VtxType::Color4f) {
                ss << "    sOut.target0 = sIn." << semantic << ";\n";
            } else if (type == VtxType::Color3f) {
                ss << "    sOut.target0 = float4(sIn." << semantic << ", 1.0f);\n";
            } else {
                std::string typeStr = FormatToShaderType(colorElement->getFormat());
                if (typeStr == "float4") {
                    ss << "    sOut.target0 = sIn." << semantic << ";\n";
                } else if (typeStr == "float3") {
                    ss << "    sOut.target0 = float4(sIn." << semantic << ", 1.0f);\n";
                } else if (typeStr == "float2") {
                    ss << "    sOut.target0 = float4(sIn." << semantic << ", 0.0f, 1.0f);\n";
                } else if (typeStr == "float") {
                    ss << "    sOut.target0 = float4(sIn." << semantic << ", 0.0f, 0.0f, 1.0f);\n";
                } else {
                    ss << "    sOut.target0 = float4(1.0f, 1.0f, 1.0f, 1.0f);\n";
                }
            }
        } else
        {
            ss << "    // 没有找到颜色属性\n";
            ss << "    sOut.target0 = float4(1.0f, 1.0f, 1.0f, 1.0f);\n";
        }

        ss << "    sOut.target1 = float4(0.0f, 0.0f, 0.0f, 0.0f);\n";
        ss << "    sOut.target2 = float4(0.0f, 0.0f, 0.0f, 0.0f);\n";
        ss << "    sOut.target3 = float4(0.0f, 0.0f, 0.0f, 0.0f);\n";
        ss << "    sOut.target4 = float4(0.0f, 0.0f, 0.0f, 0.0f);\n";
        ss << "    sOut.target5 = float4(0.0f, 0.0f, 0.0f, 0.0f);\n";
        ss << "    sOut.target6 = float4(0.0f, 0.0f, 0.0f, 0.0f);\n";
        ss << "    sOut.target7 = float4(0.0f, 0.0f, 0.0f, 0.0f);\n";

        ss << "    return sOut;\n";
        ss << "}\n";
        return ss.str();
    }
    inline std::string generatPixelShader(PixelLayout layout,std::vector<UniformLayout> uniformLayouts,RHI::ShaderBinary& binary, std::string userCode)
    {

        std::stringstream ss;
        std::map<std::string,std::pair<uint32_t,uint32_t>> uniformsLocations;
        std::map<std::string,std::pair<uint32_t,uint32_t>> constBufferLocations;
        ss << generatShaderIn(layout);
        ss << generatShaderOut();
        ss << generatConstBuffer(uniformLayouts, uniformsLocations, constBufferLocations);
        ss << generatPixelMain(layout);
        ss << userCode;
        binary.uniformLocation(uniformsLocations);
        binary.constBufferLocation(constBufferLocations);
        return ss.str();
    }
}
#endif //SHADERGENERATE_H
