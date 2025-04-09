//
// Created by Administrator on 2025/3/24.
//
#include "./VK_RasterizationPipeline.h"
#include "./VertexShader.h"
#include "./PixelShader.h"
#ifndef VK_TRADITIONALPIPELINE_H
#define VK_TRADITIONALPIPELINE_H
namespace FCT
{
    class VK_Context;
    namespace RHI
    {
        class VK_TraditionalPipeline : public VK_RasterizationPipeline
        {
        public:
            PipelineType getType() const override;
            VK_TraditionalPipeline(VK_Context* ctx);
            void addResources(IPipelineResource* resource) override;
            void create() override;
        private:
            void generateDefaultResources();
            FCT::VertexShader* m_vertexShader;
            FCT::PixelShader* m_pixelShader;
            /*
            VK_VertexShader* m_vertexShader;
            VK_PixelShader* m_pixelShader;
            */
        };

    }
}
#endif //VK_TRADITIONALPIPELINE_H
