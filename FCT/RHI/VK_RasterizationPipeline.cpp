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



