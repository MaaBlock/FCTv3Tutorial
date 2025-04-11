
#include "../FCTAPI.h"

namespace FCT
{
    std::string ShaderGenerator::generateVertexShader(
        const std::map<uint32_t, VertexLayout>& vertexLayouts,
        const PixelLayout& pixelLayout,
        const std::vector<UniformLayout>& uniformLayouts,
        RHI::ShaderBinary& binary,
        const std::string& userCode)
    {
        std::map<std::string, uint32_t> locations;
        std::map<std::string, std::pair<uint32_t, uint32_t>> uniformsLocations;
        std::map<std::string, std::pair<uint32_t, uint32_t>> constBufferLocations;

        std::stringstream ss;
        ss << generateShaderIn(vertexLayouts, locations);
        ss << generateShaderOut(pixelLayout);
        // 暂时不生成constBuffer
        // ss << generatConstBuffer(uniformLayouts, uniformsLocations, constBufferLocations);
        ss << generateVertexMain(vertexLayouts, pixelLayout);
        ss << userCode;

        binary.location(locations);
        binary.uniformLocation(uniformsLocations);
        binary.constBufferLocation(constBufferLocations);

        return ss.str();
    }

    std::string ShaderGenerator::generatePixelShader(
        const PixelLayout& layout,
        const std::vector<UniformLayout>& uniformLayouts,
        RHI::ShaderBinary& binary,
        const std::string& userCode)
    {
        std::stringstream ss;
        std::map<std::string, std::pair<uint32_t, uint32_t>> uniformsLocations;
        std::map<std::string, std::pair<uint32_t, uint32_t>> constBufferLocations;

        ss << generateShaderIn(layout);
        ss << generateShaderOut();
        // 暂时不生成constBuffer
        // ss << generatConstBuffer(uniformLayouts, uniformsLocations, constBufferLocations);
        ss << generatePixelMain(layout);
        ss << userCode;

        binary.uniformLocation(uniformsLocations);
        binary.constBufferLocation(constBufferLocations);

        return ss.str();
    }

    std::string ShaderGenerator::generateDefaultVertexMain(
        const std::map<uint32_t, VertexLayout>& vertexLayouts,
        const PixelLayout& pixelLayout)
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

                if (inElement.getType() != VtxType::Custom) {
                    auto element = pixelLayout.getElementByType(inElement.getType());
                    if (element) {
                        ss << "    sOut." << element->getSemantic() << " = sIn." << semantic << ";\n";
                    }
                } else {
                    auto element = pixelLayout.getElementBySemantic(semantic);
                    if (element) {
                        ss << "    sOut." << element->getSemantic() << " = sIn." << semantic << ";\n";
                    } else {
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

    std::string ShaderGenerator::generateDefaultPixelMain(const PixelLayout& pixelLayout)
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
        } else {
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

    std::string ShaderGenerator::generateShaderIn(
        const std::map<uint32_t, VertexLayout>& layouts,
        std::map<std::string, uint32_t>& locationMap)
    {
        std::stringstream ss;
        ss << "//FCT Input Structure\n";
        ss << "struct ShaderIn {\n";

        // 生成系统值
        ss << "    uint vertexID : SV_VertexID;\n";
        ss << "    uint instanceID : SV_InstanceID;\n\n";

        // 检查语义冲突
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

    std::string ShaderGenerator::generateShaderIn(const PixelLayout& layout)
    {
        std::stringstream ss;
        ss << "//FCT Input Structure\n";
        ss << "struct ShaderIn {\n";

        ss << "    float4 position : SV_POSITION;\n";
        ss << generateFromPixelLayout(layout);
        ss << "};\n";

        return ss.str();
    }

    std::string ShaderGenerator::generateShaderOut(const PixelLayout& layout)
    {
        std::stringstream ss;
        ss << "//FCT Output Structure\n";
        ss << "struct ShaderOut {\n";

        ss << "    float4 outPosition : SV_POSITION; \n";
        ss << generateFromPixelLayout(layout);
        ss << "};\n";

        return ss.str();
    }

    std::string ShaderGenerator::generateShaderOut()
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

    std::string ShaderGenerator::generateFromPixelLayout(const PixelLayout& layout)
    {
        std::stringstream ss;
        ss << "    //PixelLayout\n";

        for (size_t i = 0; i < layout.getElementCount(); i++) {
            const VertexElement& element = layout.getElement(i);
            std::string typeStr = FormatToShaderType(element.getFormat());
            const char* semantic = element.getSemantic();

            ss << "    [[vk::location(" << i << ")]] ";
            ss << typeStr << " " << semantic << " : " << semantic << ";\n";
        }

        return ss.str();
    }

    std::string ShaderGenerator::generateVertexMain(
        const std::map<uint32_t, VertexLayout>& vertexLayouts,
        const PixelLayout& pixelLayout)
    {
        std::stringstream ss;
        ss << "ShaderOut main(ShaderIn);\n";
        ss << "ShaderOut FCTEntry(ShaderIn sIn)\n";
        ss << " {\n";
        ss << "    ShaderOut sOut;\n";
        ss << "    \n";

        bool hasPosition = false;
        const VertexElement* posElement = nullptr;

        posElement = pixelLayout.getElementByType(VtxType::Position3f);
        if (posElement) {
            hasPosition = true;
        }
        else if ((posElement = pixelLayout.getElementByType(VtxType::Position2f))) {
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

    std::string ShaderGenerator::generatePixelMain(const PixelLayout& pixelLayout)
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

    std::string ShaderGenerator::uniformTypeToShaderType(UniformType type)
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
}