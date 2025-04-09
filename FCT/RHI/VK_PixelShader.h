//
// Created by Administrator on 2025/3/23.
//
#include "../ThirdParty.h"
#include "../RHI//PixelShader.h"
#ifndef FCT_VK_PIXELSHADER_H
#define FCT_VK_PIXELSHADER_H

namespace FCT
{
    class VK_Context;
    namespace RHI
    {
        class VK_PixelShader : public PixelShader
        {
        public:
            explicit VK_PixelShader(VK_Context* ctx);
            ~VK_PixelShader() override;

            void create() override;
            vk::PipelineShaderStageCreateInfo getStageInfo() const { return m_stageInfo; }
        private:
            void createShaderModule();
            void createPipelineStage();

            VK_Context* m_ctx;
            vk::ShaderModule m_module;
            vk::PipelineShaderStageCreateInfo m_stageInfo;
        };
    }
}

#endif //FCT_VK_PIXELSHADER_H
