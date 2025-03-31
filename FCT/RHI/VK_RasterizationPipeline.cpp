//
// Created by Administrator on 2025/3/24.
//

#include "../FCTAPI.h"

namespace FCT::RHI
{
    VK_RasterizationPipeline::VK_RasterizationPipeline(VK_Context* ctx) : m_createInfo{}
    {
        m_ctx = ctx;
        m_blendState = nullptr;
        m_rasterizationState = nullptr;
        m_viewportState = nullptr;
        m_inputLayout = nullptr;
    }

    VK_RasterizationPipeline::~VK_RasterizationPipeline()
    {
        FCT_SAFE_RELEASE(m_blendState);
        FCT_SAFE_RELEASE(m_rasterizationState);
        FCT_SAFE_RELEASE(m_viewportState);
        FCT_SAFE_RELEASE(m_inputLayout);
    }

    void VK_RasterizationPipeline::addResources(IPipelineResource* resource)
    {
        switch (resource->getType())
        {
        case PipelineResourceType::BlendState:
            m_blendState = static_cast<VK_BlendState*>(resource);
            m_blendState->addRef();
            break;
        case PipelineResourceType::RasterizationState:
            m_rasterizationState = static_cast<VK_RasterizationState*>(resource);
            m_rasterizationState->addRef();
            break;
        case PipelineResourceType::ViewportState:
            m_viewportState = static_cast<VK_ViewportState*>(resource);
            m_viewportState->addRef();
            break;
        case PipelineResourceType::InputLayout:
            m_inputLayout = static_cast<VK_InputLayout*>(resource);
            m_inputLayout->addRef();
            break;
        default:
            ferr << "Unsupported pipeline resource type: " << (unsigned int)(resource->getType()) << std::endl;
            break;
        }
    }

    void VK_RasterizationPipeline::create()
    {
        generateDefaultResources();

        //fill resource
        std::vector<vk::DynamicState> dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };
        vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.pDynamicStates = dynamicStates.data();
        m_createInfo.pDynamicState = &dynamicState;
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        auto pipelineLayout = m_ctx->getDevice().createPipelineLayout(pipelineLayoutInfo);
        m_createInfo.renderPass = static_cast<VK_PassGroup*>(m_pass->passGroup())->getRenderPass();
        m_createInfo.subpass = static_cast<VK_PassGroup*>(m_pass->passGroup())->getPassIndex(m_pass);
        m_createInfo.layout = pipelineLayout;
        vk::PipelineVertexInputStateCreateInfo vertexInputState{};
        vertexInputState.vertexBindingDescriptionCount = 0;
        vertexInputState.pVertexBindingDescriptions = nullptr;
        vertexInputState.vertexAttributeDescriptionCount = m_inputLayout->attributeDescriptions().size();
        vertexInputState.pVertexAttributeDescriptions = m_inputLayout->attributeDescriptions().data();
        m_createInfo.pVertexInputState = &vertexInputState;
        m_createInfo.pRasterizationState = &m_rasterizationState->rasterizationStateCreateInfo();
        m_createInfo.pMultisampleState = &m_rasterizationState->multisampleStateCreateInfo();
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState{};
        inputAssemblyState.topology = vk::PrimitiveTopology::eTriangleList;
        inputAssemblyState.primitiveRestartEnable = VK_FALSE;
        m_createInfo.pInputAssemblyState = &inputAssemblyState;
        m_createInfo.pViewportState = &m_viewportState->viewportStateCreateInfo();
        m_createInfo.pDepthStencilState = nullptr;
        m_createInfo.pColorBlendState = &m_blendState->colorBlendStateCreateInfo();
        m_createInfo.pDynamicState = nullptr;

        m_createInfo.stageCount = m_shaderStages.size();
        m_createInfo.pStages = m_shaderStages.data();
        m_pipeline = m_ctx->getDevice().createGraphicsPipeline(nullptr,m_createInfo).value;
    }

    void VK_RasterizationPipeline::bindPass(Pass* pass)
    {
        m_pass = static_cast<VK_Pass*>(pass);
    }

    void VK_RasterizationPipeline::generateDefaultResources()
    {
        if (!m_blendState)
        {
            m_blendState = new VK_BlendState(m_ctx);
            m_blendState->create();
        }
        if (!m_rasterizationState)
        {
            m_rasterizationState = new VK_RasterizationState(m_ctx);
            m_rasterizationState->create();
        }
        if (!m_viewportState)
        {
            m_viewportState = new VK_ViewportState(m_ctx);
            m_viewportState->create();
        }
        if (!m_inputLayout)
        {
            m_inputLayout = new VK_InputLayout(m_ctx);
            m_inputLayout->create();
        }
    }
}



