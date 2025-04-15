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
            createDescriptorSetLayouts();
            m_shaderStages.push_back(static_cast<VK_VertexShader*>(m_vertexShader->vertexShader())->getStageInfo());
            m_shaderStages.push_back(static_cast<VK_PixelShader*>(m_pixelShader->pixelShader())->getStageInfo());
            VK_RasterizationPipeline::create();
        }
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
            }
            if (!m_pixelShader)
            {
                m_pixelShader = m_ctx->createPixelShader();
                m_pixelShader->pixelLayout(m_pixelLayout);
                m_pixelShader->create();
            }
            if (!m_inputLayout)
            {
                m_inputLayout = static_cast<VK_InputLayout*>(m_vertexShader->createBindedInputLayout());
                m_inputLayout->create();
            }
        }

        void VK_TraditionalPipeline::createDescriptorSetLayouts()
        {
            std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> setBindings;

            if (m_vertexShader)
            {
                auto shaderBinary = m_vertexShader->binaryCode();
                auto constBufferLocations = shaderBinary.constBufferLocations();
                auto constBufferLayouts = shaderBinary.constBufferLayout();

                for (const auto& [name, location] : constBufferLocations)
                {
                    uint32_t set = location.first;
                    uint32_t binding = location.second;

                    auto layoutIt = constBufferLayouts.find(name);
                    if (layoutIt == constBufferLayouts.end()) {
                        continue;
                    }

                    const UniformLayout& layout = layoutIt->second;
                    vk::ShaderStageFlags stageFlags = ConvertToVkShaderStageFlags(layout.getShaderStages());

                    vk::DescriptorSetLayoutBinding layoutBinding(
                        binding,
                        vk::DescriptorType::eUniformBuffer,
                        1,
                        stageFlags,
                        nullptr
                    );

                    setBindings[set].push_back(layoutBinding);
                }
            }

            if (m_pixelShader)
            {
                auto shaderBinary = m_pixelShader->binaryCode();
                auto constBufferLocations = shaderBinary.constBufferLocations();
                auto constBufferLayouts = shaderBinary.constBufferLayout();

                for (const auto& [name, location] : constBufferLocations)
                {
                    uint32_t set = location.first;
                    uint32_t binding = location.second;

                    auto layoutIt = constBufferLayouts.find(name);
                    if (layoutIt == constBufferLayouts.end()) {
                        continue;
                    }

                    const UniformLayout& layout = layoutIt->second;
                    vk::ShaderStageFlags stageFlags = ConvertToVkShaderStageFlags(layout.getShaderStages());

                    vk::DescriptorSetLayoutBinding layoutBinding(
                        binding,
                        vk::DescriptorType::eUniformBuffer,
                        1,
                        stageFlags,
                        nullptr
                    );

                    setBindings[set].push_back(layoutBinding);
                }
            }

            m_descriptorSetLayouts.clear();
            for (const auto& [set, bindings] : setBindings) {
                vk::DescriptorSetLayoutCreateInfo layoutInfo(
                    vk::DescriptorSetLayoutCreateFlags(),
                    static_cast<uint32_t>(bindings.size()),
                    bindings.data()
                );

                try {
                    m_descriptorSetLayouts[set] = m_ctx->getDevice().createDescriptorSetLayout(layoutInfo);
                } catch (const vk::SystemError& e) {
                    throw std::runtime_error("Failed to create descriptor set layout: " + std::string(e.what()));
                }
            }

            m_descriptorSetLayoutsArr.clear();
            if (!m_descriptorSetLayouts.empty()) {
                uint32_t maxSet = 0;
                for (const auto& [set, layout] : m_descriptorSetLayouts) {
                    maxSet = std::max(maxSet, set);
                }

                vk::DescriptorSetLayoutCreateInfo emptyLayoutInfo(
                    vk::DescriptorSetLayoutCreateFlags(),
                    0,
                    nullptr
                );
                vk::DescriptorSetLayout emptyLayout;
                try {
                    emptyLayout = m_ctx->getDevice().createDescriptorSetLayout(emptyLayoutInfo);
                } catch (const vk::SystemError& e) {
                    throw std::runtime_error("Failed to create empty descriptor set layout: " + std::string(e.what()));
                }

                m_descriptorSetLayoutsArr.resize(maxSet + 1, emptyLayout);

                for (const auto& [set, layout] : m_descriptorSetLayouts) {
                    m_descriptorSetLayoutsArr[set] = layout;
                }

                m_emptyLayout = emptyLayout;
            }

            vk::PipelineLayoutCreateInfo pipelineLayoutInfo(
                vk::PipelineLayoutCreateFlags(),
                static_cast<uint32_t>(m_descriptorSetLayoutsArr.size()),
                m_descriptorSetLayoutsArr.empty() ? nullptr : m_descriptorSetLayoutsArr.data(),
                0,
                nullptr
            );

            try {
                m_pipelineLayout = m_ctx->getDevice().createPipelineLayout(pipelineLayoutInfo);
            } catch (const vk::SystemError& e) {
                throw std::runtime_error("Failed to create pipeline layout: " + std::string(e.what()));
            }
        }
    }
}
