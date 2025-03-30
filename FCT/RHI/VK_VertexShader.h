//
// Created by Administrator on 2025/3/23.
//
#include "../ThirdParty.h"
#include "./VertexShader.h"
#ifndef VK_VERTEXSHADER_H
#define VK_VERTEXSHADER_H
namespace FCT
{
    class VK_Context;
    class VK_VertexShader : public VertexShader{
    public:
        VK_VertexShader(VK_Context* ctx);
        ~VK_VertexShader();
        void create() override;
        void bind() override;
        void unbind() override;
    private:
        void createShaderModule();
        void createPipelineStage();
        VK_Context* m_ctx;
        VkShaderModule m_module;
        vk::PipelineShaderStageCreateInfo m_stageInfo;
    };

}


#endif //VK_VERTEXSHADER_H
