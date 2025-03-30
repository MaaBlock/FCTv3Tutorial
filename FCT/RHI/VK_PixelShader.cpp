//
// Created by Administrator on 2025/3/23.
//
#include "../FCTAPI.h"
namespace FCT
{
    VK_PixelShader::VK_PixelShader(VK_Context* ctx)
        : m_ctx(ctx), m_module(VK_NULL_HANDLE)
    {

    }

    VK_PixelShader::~VK_PixelShader()
    {
        if (m_module != VK_NULL_HANDLE) {
            m_ctx->getDevice().destroyShaderModule(m_module);
        }
    }

    void VK_PixelShader::create()
    {
        createShaderModule();
        createPipelineStage();
    }

    void VK_PixelShader::bind()
    {

    }

    void VK_PixelShader::unbind()
    {

    }

    void VK_PixelShader::createShaderModule()
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.codeSize = m_code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(m_code.data());
        m_module = m_ctx->getDevice().createShaderModule(createInfo);
    }

    void VK_PixelShader::createPipelineStage()
    {
        m_stageInfo.stage = vk::ShaderStageFlagBits::eFragment;
        m_stageInfo.module = m_module;
        m_stageInfo.pName = "main";
    }

}