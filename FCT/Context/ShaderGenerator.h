//
// Created by Administrator on 2025/4/11.
//
#include "./ShaderGenerate.h"
#ifndef FCT_SHADERGENERATOR_H
#define FCT_SHADERGENERATOR_H
namespace FCT
{
    class ShaderGenerator
    {
    public:
        ShaderGenerator() = default;

        std::string generateVertexShader(
            const std::map<uint32_t, VertexLayout>& vertexLayouts,
            const PixelLayout& pixelLayout,
            const std::vector<UniformLayout>& uniformLayouts,
            RHI::ShaderBinary& binary,
            const std::string& userCode);

        std::string generatePixelShader(
            const PixelLayout& layout,
            const std::vector<UniformLayout>& uniformLayouts,
            RHI::ShaderBinary& binary,
            const std::string& userCode);

        std::string generateDefaultVertexMain(
            const std::map<uint32_t, VertexLayout>& vertexLayouts,
            const PixelLayout& pixelLayout);

        std::string generateDefaultPixelMain(const PixelLayout& pixelLayout);

    protected:
        std::string generateShaderIn(
            const std::map<uint32_t, VertexLayout>& layouts,
            std::map<std::string, uint32_t>& locationMap);

        std::string generateShaderIn(const PixelLayout& layout);

        std::string generateShaderOut(const PixelLayout& layout);

        std::string generateShaderOut();

        std::string generateFromPixelLayout(const PixelLayout& layout);

        std::string generateVertexMain(
            const std::map<uint32_t, VertexLayout>& vertexLayouts,
            const PixelLayout& pixelLayout);

        std::string generatePixelMain(const PixelLayout& pixelLayout);

        std::string uniformTypeToShaderType(UniformType type);
    };
}
#endif //FCT_SHADERGENERATOR_H