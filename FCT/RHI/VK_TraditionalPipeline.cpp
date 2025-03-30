//
// Created by Administrator on 2025/3/24.
//
#include "../FCTAPI.h"
namespace FCT
{
    namespace RHI
    {
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
                    m_vertexShader = static_cast<VK_VertexShader*>(resource);
                    m_vertexShader->addRef();
                    break;
                case PipelineResourceType::PixelShader:
                    m_pixelShader = static_cast<VK_PixelShader*>(resource);
                    m_pixelShader->addRef();
                    break;
                default:
                    VK_RasterizationPipeline::addResources(resource);
                    break;
            }
        }

        void VK_TraditionalPipeline::create()
        {
            generateDefaultResources();
            VK_RasterizationPipeline::create();
        }

        void VK_TraditionalPipeline::generateDefaultResources()
        {
            if (!m_vertexShader)
            {
                m_vertexShader = new VK_VertexShader(m_ctx);
                m_vertexShader->create();
            }
            if (!m_pixelShader)
            {
                m_pixelShader = new VK_PixelShader(m_ctx);
                m_pixelShader->create();
            }
        }
    }
}
