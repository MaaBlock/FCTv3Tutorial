//
// Created by Administrator on 2025/3/24.
//
#include "./RasterizationPipeline.h"
#include "./VK_BlendState.h"
#include "./VK_InputLayout.h"
#include "./VK_ViewportState.h"
#include "./VK_RasterizationState.h"
#include "VK_PassGroup.h"
#ifndef VK_RASTERIZATIONPIPELINE_H
#define VK_RASTERIZATIONPIPELINE_H


namespace FCT
{
    class VK_Context;
    namespace RHI
    {
        class VK_Pass;

        class VK_RasterizationPipeline : public RasterizationPipeline{
        public:
            VK_RasterizationPipeline(VK_Context* ctx);
            virtual ~VK_RasterizationPipeline();
            virtual void addResources(IPipelineResource* resource);
            void create() override;
            void bindPass(Pass* pass) override;
        protected:
            void generateDefaultResources();
            std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStages;
            VK_Context* m_ctx;
            VK_BlendState* m_blendState;
            VK_InputLayout* m_inputLayout;
            VK_ViewportState* m_viewportState;
            VK_RasterizationState* m_rasterizationState;
            vk::GraphicsPipelineCreateInfo m_createInfo;
            VK_Pass* m_pass;
            vk::Pipeline m_pipeline;
        };
    }
}



#endif //VK_RASTERIZATIONPIPELINE_H
