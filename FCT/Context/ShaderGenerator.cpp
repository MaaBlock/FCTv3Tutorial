
#include "../FCTAPI.h"

namespace FCT
{
    void ShaderGenerator::ResourceLayoutToElements(ResourceLayout& resourceLayout,
    std::vector<TextureElement>& textureElements, std::vector<SamplerElement>& samplerElements)
    {
        textureElements.clear();
        samplerElements.clear();
        size_t textureCount = resourceLayout.getTextureCount();
        size_t samplerCount = resourceLayout.getSamplerCount();
        textureElements.reserve(textureCount);
        samplerElements.reserve(samplerCount);
        for (size_t i = 0; i < textureCount; ++i) {
            textureElements.push_back(resourceLayout.getTexture(i));
        }
        for (size_t i = 0; i < samplerCount; ++i) {
            samplerElements.push_back(resourceLayout.getSampler(i));
        }
    }

    std::string ShaderGenerator::generateVertexShader(const std::map<uint32_t, VertexLayout>& vertexLayouts,
                                                      const PixelLayout& pixelLayout, const std::vector<ConstLayout>& uniformLayouts, RHI::ShaderBinary& binary,
                                                      ResourceLayout& resourceLayout, const std::string& userCode)
    {
        std::map<std::string, uint32_t> locations;
        std::map<std::string, std::pair<uint32_t, uint32_t>> uniformsLocations;
        std::map<std::string, std::pair<uint32_t, uint32_t>> constBufferLocations;

        std::vector<TextureElement> texturesElements;
        std::vector<SamplerElement> samplersElements;
        ResourceLayoutToElements(resourceLayout, texturesElements, samplersElements);

        std::stringstream ss;
        ss << generateShaderIn(vertexLayouts, locations);
        ss << generateShaderOut(pixelLayout);
        ss << generateConstBuffer(binary,uniformLayouts);
        ss << generateTexturesAndSamplers(binary,texturesElements, samplersElements);
        ss << generateVertexMain(vertexLayouts, pixelLayout);
        ss << userCode;

        binary.location(locations);

        return ss.str();
    }

    std::string ShaderGenerator::generatePixelShader(const PixelLayout& layout,
        const std::vector<ConstLayout>& uniformLayouts, RHI::ShaderBinary& binary, ResourceLayout& resourceLayout,
        const std::string& userCode)
    {
        std::stringstream ss;
        std::vector<TextureElement> texturesElements;
        std::vector<SamplerElement> samplersElements;
        ResourceLayoutToElements(resourceLayout, texturesElements, samplersElements);

        ss << generateShaderIn(layout);
        ss << generateShaderOut();
        ss << generateConstBuffer(binary,uniformLayouts);
        ss << generateTexturesAndSamplers(binary,texturesElements, samplersElements);
        ss << generatePixelMain(layout);
        ss << userCode;


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

        posElement = pixelLayout.getElementByType(VtxType::Position4f);
        if (posElement) {
            hasPosition = true;
        }

        else if ((posElement = pixelLayout.getElementByType(VtxType::Position3f))) {
            hasPosition = true;
        }

        else if ((posElement = pixelLayout.getElementByType(VtxType::Position2f))) {
            hasPosition = true;
        }

        ss << "    sOut = main(sIn);\n";

        if (hasPosition && posElement) {
            const char* semantic = posElement->getSemantic();
            VtxType type = posElement->getType();

            if (type == VtxType::Position4f) {
                ss << "    sOut.outPosition = sOut." << semantic << ";\n";
            } else if (type == VtxType::Position3f) {
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

    std::pair<uint32_t, uint32_t> ShaderGenerator::allocateTextureBinding(const TextureElement& texture) {
        UpdateFrequency frequency = texture.getUpdateFrequency();
        ShaderStages stages = texture.getShaderStages();

        uint32_t set;
        switch (frequency) {
        case UpdateFrequency::Static:
            set = 0;
            break;
        case UpdateFrequency::PerFrame:
            set = 1;
            break;
        case UpdateFrequency::PerObject:
            set = 2;
            break;
        case UpdateFrequency::Dynamic:
            set = 3;
            break;
        default:
            set = 0;
            break;
        }

        uint32_t binding;
        bool found = false;

        for (const auto& [storedTexture, setBinding] : m_textureSetBindings) {
            if (storedTexture == texture) {
                set = setBinding.first;
                binding = setBinding.second;
                found = true;
                break;
            }
        }

        if (!found) {
            binding = m_frequencyBindingCount[frequency]++;
            m_textureSetBindings.push_back({texture, {set, binding}});
        }

        return {set, binding};
    }
    std::pair<uint32_t, uint32_t> ShaderGenerator::allocateSamplerBinding(const SamplerElement& sampler) {
        UpdateFrequency frequency = sampler.getUpdateFrequency();
        ShaderStages stages = sampler.getShaderStages();

        uint32_t set;
        switch (frequency) {
        case UpdateFrequency::Static:
            set = 0;
            break;
        case UpdateFrequency::PerFrame:
            set = 1;
            break;
        case UpdateFrequency::PerObject:
            set = 2;
            break;
        case UpdateFrequency::Dynamic:
            set = 3;
            break;
        default:
            set = 0;
            break;
        }

        uint32_t binding;
        bool found = false;

        for (const auto& [storedSampler, setBinding] : m_samplerSetBindings) {
            if (storedSampler == sampler) {
                set = setBinding.first;
                binding = setBinding.second;
                found = true;
                break;
            }
        }

        if (!found) {
            binding = m_frequencyBindingCount[frequency]++;
            m_samplerSetBindings.push_back({sampler, {set, binding}});
        }

        return {set, binding};
    }
    std::string ShaderGenerator::generateTexturesAndSamplers(RHI::ShaderBinary& binary,
    const std::vector<TextureElement>& textures, const std::vector<SamplerElement>& samplers)
    {
        std::stringstream ss;
        ss << "//FCT Textures and Samplers\n";

        for (const auto& texture : textures) {
            auto [set, binding] = allocateTextureBinding(texture);

            binary.addTextureLocation(texture, set, binding);

            std::string textureType;
            switch (texture.getType()) {
            case TextureType::Texture2D:
                textureType = "Texture2D";
                break;
            case TextureType::Texture3D:
                textureType = "Texture3D";
                break;
            case TextureType::TextureCube:
                textureType = "TextureCube";
                break;
            case TextureType::Texture2DArray:
                textureType = "Texture2DArray";
                break;
            case TextureType::DepthTexture:
                textureType = "Texture2D";
                break;
            default:
                textureType = "Texture2D";
                break;
            }

            ss << "[[vk::binding(" << binding << ", " << set << ")]] ";
            ss << textureType << "<float4> " << texture.getName()
               << " : register(t" << binding << ", space" << set << ");\n";
        }

        ss << "\n";

        for (const auto& sampler : samplers) {
            auto [set, binding] = allocateSamplerBinding(sampler);

            binary.addSamplerLocation(sampler, set, binding);

            ss << "[[vk::binding(" << binding << ", " << set << ")]] ";
            ss << "SamplerState " << sampler.getName()
               << " : register(s" << binding << ", space" << set << ");\n";
        }

        return ss.str();
    }

    std::string ShaderGenerator::uniformTypeToShaderType(ConstType type)
    {
        switch (type) {
        case ConstType::ModelMatrix:
        case ConstType::ViewMatrix:
        case ConstType::ProjectionMatrix:
        case ConstType::MVPMatrix:
        case ConstType::Mat4:
            return "float4x4";
        case ConstType::Mat3:
            return "float3x3";
        case ConstType::Vec4:
            return "float4";
        case ConstType::Vec3:
            return "float3";
        case ConstType::Vec2:
            return "float2";
        case ConstType::Float:
            return "float";
        case ConstType::Int:
            return "int";
        case ConstType::Bool:
            return "bool";
        case ConstType::Texture2D:
        case ConstType::TextureCube:
        case ConstType::Custom:
        default:
            return "float4";
        }
    }
    std::string ShaderGenerator::generateConstBuffer(RHI::ShaderBinary& binary,
        const std::vector<ConstLayout>& uniforms)
    {
        std::stringstream ss;
        ss << "//FCT Constant Buffers\n";

        for (const auto& layout : uniforms) {
            UpdateFrequency frequency = layout.getUpdateFrequency();

            uint32_t set;
            switch (frequency) {
            case UpdateFrequency::Static:
                set = 0;
                break;
            case UpdateFrequency::PerFrame:
                set = 1;
                break;
            case UpdateFrequency::PerObject:
                set = 2;
                break;
            case UpdateFrequency::Dynamic:
                set = 3;
                break;
            default:
                set = 0;
                break;
            }

            uint32_t binding;
            bool found = false;

            for (const auto& [storedLayout, setBinding] : m_layoutSetBindings) {
                if (storedLayout == layout) {
                    set = setBinding.first;
                    binding = setBinding.second;
                    found = true;
                    break;
                }
            }

            if (!found) {
                binding = m_frequencyBindingCount[frequency]++;

                m_layoutSetBindings.push_back({layout, {set, binding}});
            }

            binary.addConstBufferLocation(layout, set, binding);

            ss << "[[vk::binding(" << binding << ", " << set << ")]] ";
            // ss << "[[vk::set(" << set << ")]] ";
            ss << "cbuffer " << layout.getName() << " : register(b" << binding << ", space" << set << ") {\n";

            for (size_t i = 0; i < layout.getElementCount(); i++) {
                const ConstElement& element = layout.getElement(i);
                std::string typeStr = uniformTypeToShaderType(element.getType());
                const char* name = element.getName();

                ss << "    " << typeStr << " " << name << ";\n";
            }

            ss << "};\n\n";
        }

        return ss.str();
    }
}
