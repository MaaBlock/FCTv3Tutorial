//
// Created by Administrator on 2025/4/11.
//
#include "./ResourceLayout.h"
#include "./ShaderGenerate.h"
#ifndef FCT_SHADERGENERATOR_H
#define FCT_SHADERGENERATOR_H
namespace FCT
{
    /*
     *todo:
     *  1.对于从外部加载S haderBinary,用spirv-cross,
     *    对加载的Shader查找一下有没有对应的layout，
     *    有用哪个的binding，没用就分配新的binding，
     *    然后用spirv-cross反射改变shader里面的binding
     *  2.对于外部加载的，通过判断name和布局是否一一 相等
     *    来判断是否相等，如果布局 相等，但是名字不 相等
     *    可以手动更改配置文件里面的名字来使得FCT判断其 相等
     *     故对于ShaderBinary的存储，layout的名字应该需要好改，
     *     应当只存储一次layout的名字
     */
    class ShaderGenerator
    {
    public:
        ShaderGenerator() = default;
        void ResourceLayoutToElements(ResourceLayout& resourceLayout,std::vector<TextureElement>& textureElements,std::vector<SamplerElement>& samplerElements);
        std::string generateVertexShader(
            const std::map<uint32_t, VertexLayout>& vertexLayouts,
            const PixelLayout& pixelLayout,
            const std::vector<ConstLayout>& uniformLayouts,
            RHI::ShaderBinary& binary,
            ResourceLayout& resourceLayout,
            const std::string& userCode);

        std::string generatePixelShader(
            const PixelLayout& layout,
            const std::vector<ConstLayout>& uniformLayouts,
            RHI::ShaderBinary& binary,
            ResourceLayout& resourceLayout,
            const std::string& userCode);

        std::string generateDefaultVertexMain(
            const std::map<uint32_t, VertexLayout>& vertexLayouts,
            const PixelLayout& pixelLayout);

        std::string generateDefaultPixelMain(const PixelLayout& pixelLayout);

        std::pair<uint32_t, uint32_t> getLayoutBinding(ConstLayout layout)
        {
            for (const auto& pair : m_layoutSetBindings) {
                if (pair.first == layout) {
                    return pair.second;
                }
            }
            //todo:alloc new binding
            return std::make_pair(0, 0);
        }
        std::pair<uint32_t, uint32_t> getTextureBinding(TextureElement texture)
        {
            for (const auto& pair : m_textureSetBindings) {
                if (pair.first == texture) {
                    return pair.second;
                }
            }
            //todo:alloc new binding
            return std::make_pair(0, 0);
        }

        std::pair<uint32_t, uint32_t> getSamplerBinding(SamplerElement sampler)
        {
            for (const auto& pair : m_samplerSetBindings) {
                if (pair.first == sampler) {
                    return pair.second;
                }
            }
            //todo:alloc new binding
            return std::make_pair(0, 0);
        }
        TextureElement findTextureElementByName(std::string name)
        {
            for (const auto& pair : m_textureSetBindings) {
                if (pair.first.getName() == name) {
                    return pair.first;
                }
            }
            return TextureElement();
        }
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
        std::string generateTexturesAndSamplers(RHI::ShaderBinary& binary, const std::vector<TextureElement>& textures,
                                                const std::vector<SamplerElement>& samplers);

        std::string uniformTypeToShaderType(ConstType type);

        std::string generateConstBuffer(RHI::ShaderBinary& binary,const std::vector<ConstLayout>& uniforms);

        std::vector<std::pair<ConstLayout, std::pair<uint32_t, uint32_t>>> m_layoutSetBindings;
        std::unordered_map<UpdateFrequency, uint32_t> m_frequencyBindingCount;

        std::vector<std::pair<TextureElement, std::pair<uint32_t, uint32_t>>> m_textureSetBindings;
        std::vector<std::pair<SamplerElement, std::pair<uint32_t, uint32_t>>> m_samplerSetBindings;
    };
}
#endif //FCT_SHADERGENERATOR_H