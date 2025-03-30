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

    class VK_PixelShader : public PixelShader
    {
    public:
        explicit VK_PixelShader(VK_Context* ctx);
        ~VK_PixelShader() override;

        void create() override;
        void bind() override;
        void unbind() override;
    private:
        void createShaderModule();
        void createPipelineStage();

        VK_Context* m_ctx;
        vk::ShaderModule m_module;
        std::vector<char> m_code;
        vk::PipelineShaderStageCreateInfo m_stageInfo;
    };
}

#endif //FCT_VK_PIXELSHADER_H
