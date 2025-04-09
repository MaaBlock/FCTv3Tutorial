//
// Created by Administrator on 2025/3/23.
//
#include "../ThirdParty.h"
#include "./VertexShader.h"
#ifndef FCT_RHI_VK_VERTEXSHADER_H
#define FCT_RHI_VK_VERTEXSHADER_H
namespace FCT
{
    class VK_Context;
    namespace RHI
    {
        class VK_VertexShader : public VertexShader{
        public:
            VK_VertexShader(VK_Context* ctx);
            ~VK_VertexShader();
            void create() override;
            vk::PipelineShaderStageCreateInfo getStageInfo();
        private:
            void createShaderModule();
            void createPipelineStage();
            void performReflection();
            VK_Context* m_ctx;
            vk::ShaderModule m_module;
            vk::PipelineShaderStageCreateInfo m_stageInfo;
        };
    }

}


#endif //FCT_RHI_VK_VERTEXSHADER_H
