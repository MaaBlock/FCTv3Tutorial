//
// Created by Administrator on 2025/3/24.
//
#include "../FCTAPI.h"
namespace FCT
{
    namespace RHI
    {
        PipelineType VK_TraditionalPipeline::getType() const
        {
            return PipelineType::Traditional;
        }

        VK_TraditionalPipeline::VK_TraditionalPipeline(VK_Context* ctx) : VK_RasterizationPipeline(ctx)
        {
            m_pixelShader = nullptr;
            m_vertexShader = nullptr;
        }

        void VK_TraditionalPipeline::addResources(IPipelineResource* resource)
        {
            switch (resource->getType())
            {
                case PipelineResourceType::VertexShader:
                    m_vertexShader = static_cast<FCT::VertexShader*>(resource);
                    m_vertexShader->addRef();
                    /*
                    m_vertexShader = static_cast<VK_VertexShader*>(static_cast<FCT::VertexShader*>(resource)->vertexShader());
                    m_vertexShader->addRef();
                    */
                    break;
                case PipelineResourceType::PixelShader:
                    m_pixelShader = static_cast<FCT::PixelShader*>(resource);
                    m_pixelShader->addRef();
                    /*
                    m_pixelShader = static_cast<VK_PixelShader*>(static_cast<FCT::PixelShader*>(resource)->pixelShader());
                    m_pixelShader->addRef();*/
                    break;
                default:
                    VK_RasterizationPipeline::addResources(resource);
                    break;
            }
        }

        void VK_TraditionalPipeline::create()
        {
            generateDefaultResources();
            m_shaderStages.push_back(static_cast<VK_VertexShader*>(m_vertexShader->vertexShader())->getStageInfo());
            m_shaderStages.push_back(static_cast<VK_PixelShader*>(m_pixelShader->pixelShader())->getStageInfo());
            VK_RasterizationPipeline::create();
        }
        /*
        std::vector<vk::DescriptorSetLayout> VK_TraditionalPipeline::createDescriptorSetLayouts()
        {
            // 使用 uniform 名称和着色器阶段的组合作为键
            // 存储格式：{uniformName, shaderStage} -> {set, binding, descriptorType}
            std::map<std::pair<std::string, vk::ShaderStageFlagBits>, std::tuple<uint32_t, uint32_t, vk::DescriptorType>> uniformStageInfo;

            // 从顶点着色器获取 uniform 位置
            if (m_vertexShader) {
                RHI::ShaderBinary vsBinary = static_cast<VK_VertexShader*>(m_vertexShader->vertexShader())->binaryCode();
                auto vsLocations = vsBinary.uniformLocations();

                for (const auto& [name, location] : vsLocations) {
                    uint32_t set = location.first;
                    uint32_t binding = location.second;
                    vk::DescriptorType type = vk::DescriptorType::eUniformBuffer; // 假设所有的都是 UBO

                    // 使用 uniform 名称和顶点着色器阶段作为键
                    std::pair<std::string, vk::ShaderStageFlagBits> key = {name, vk::ShaderStageFlagBits::eVertex};

                    // 添加 uniform 信息
                    uniformStageInfo[key] = {set, binding, type};
                }
            }

            // 从像素着色器获取 uniform 位置
            if (m_pixelShader) {
                RHI::ShaderBinary psBinary = static_cast<VK_PixelShader*>(m_pixelShader->pixelShader())->binaryCode();
                auto psLocations = psBinary.uniformLocations();

                for (const auto& [name, location] : psLocations) {
                    uint32_t set = location.first;
                    uint32_t binding = location.second;
                    vk::DescriptorType type = vk::DescriptorType::eUniformBuffer; // 假设所有的都是 UBO

                    // 使用 uniform 名称和片段着色器阶段作为键
                    std::pair<std::string, vk::ShaderStageFlagBits> key = {name, vk::ShaderStageFlagBits::eFragment};

                    // 添加 uniform 信息
                    uniformStageInfo[key] = {set, binding, type};
                }
            }

            // 将 uniform 信息转换为 set/binding 信息
            // 使用 {set, binding} 作为键，合并相同 set 和 binding 的着色器阶段
            std::map<std::pair<uint32_t, uint32_t>, std::pair<vk::DescriptorType, vk::ShaderStageFlags>> setBindingInfo;

            for (const auto& [key, info] : uniformStageInfo) {
                auto [name, stage] = key;
                auto [set, binding, type] = info;

                std::pair<uint32_t, uint32_t> setBindingKey = {set, binding};

                // 检查这个 set 和 binding 是否已经存在
                if (setBindingInfo.find(setBindingKey) != setBindingInfo.end()) {
                    // 已存在，检查类型是否一致
                    if (setBindingInfo[setBindingKey].first != type) {
                        // 类型不一致，这是一个错误情况
                        throw std::runtime_error("Descriptor type mismatch at set " + std::to_string(set) +
                                                ", binding " + std::to_string(binding));
                    }

                    // 合并着色器阶段
                    setBindingInfo[setBindingKey].second |= stage;
                } else {
                    // 不存在，添加新的信息
                    setBindingInfo[setBindingKey] = {type, stage};
                }
            }

            // 创建描述符布局
            std::vector<vk::DescriptorSetLayout> layouts;

            // 确保 set 是按顺序的
            uint32_t maxSet = 0;
            for (const auto& [setBinding, _] : setBindingInfo) {
                maxSet = std::max(maxSet, setBinding.first);
            }

            layouts.resize(maxSet + 1);

            // 按 set 分组
            std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> setToBindings;

            for (const auto& [setBinding, info] : setBindingInfo) {
                auto [set, binding] = setBinding;
                auto [type, stageFlags] = info;

                vk::DescriptorSetLayoutBinding layoutBinding;
                layoutBinding.binding = binding;
                layoutBinding.descriptorType = type;
                layoutBinding.descriptorCount = 1;
                layoutBinding.stageFlags = stageFlags;
                layoutBinding.pImmutableSamplers = nullptr;

                setToBindings[set].push_back(layoutBinding);
            }

            // 为每个 set 创建描述符布局
            for (const auto& [set, bindings] : setToBindings) {
                vk::DescriptorSetLayoutCreateInfo layoutInfo;
                layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
                layoutInfo.pBindings = bindings.data();

                layouts[set] = m_ctx->device().createDescriptorSetLayout(layoutInfo);
            }

            return layouts;
        }*/

        void VK_TraditionalPipeline::generateDefaultResources()
        {
            FCT::VertexShader* vs = nullptr;
            if (!m_vertexShader)
            {
                m_vertexShader = m_ctx->createVertexShader();
                for (auto pair : m_vertexLayouts)
                {
                    m_vertexShader->addLayout(pair.first, pair.second);
                }
                m_vertexShader->pixelLayout(m_pixelLayout);
                m_vertexShader->create();
                if (!m_inputLayout)
                {
                    m_inputLayout = static_cast<VK_InputLayout*>(m_vertexShader->createBindedInputLayout());
                    m_inputLayout->create();
                }
            }
            if (!m_pixelShader)
            {
                m_pixelShader = m_ctx->createPixelShader();
                m_pixelShader->pixelLayout(m_pixelLayout);
                m_pixelShader->create();
            }
        }
    }
}
