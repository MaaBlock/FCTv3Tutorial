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
