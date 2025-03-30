//
// Created by Administrator on 2025/3/24.
//
#include "./RasterizationPipeline.h"
#include "./VK_BlendState.h"
#include "./VK_InputLayout.h"
#include "./VK_ViewportState.h"
#include "./VK_RasterizationState.h"
#ifndef VK_RASTERIZATIONPIPELINE_H
#define VK_RASTERIZATIONPIPELINE_H


namespace FCT
{
    class VK_Context;
    namespace RHI
    {
        class VK_RasterizationPipeline : public RasterizationPipeline{
        public:
            VK_RasterizationPipeline(VK_Context* ctx);
            virtual ~VK_RasterizationPipeline();
            virtual void addResources(IPipelineResource* resource);
            void create() override;
        protected:
            virtual void generateDefaultResources();
            VK_Context* m_ctx;
            VK_BlendState* m_blendState;
            VK_InputLayout* m_inputLayout;
            VK_ViewportState* m_viewportState;
            VK_RasterizationState* m_rasterizationState;
            vk::GraphicsPipelineCreateInfo m_createInfo;
        };
    }
}



#endif //VK_RASTERIZATIONPIPELINE_H
