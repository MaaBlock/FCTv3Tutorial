//
// Created by Administrator on 2025/3/23.
//
#include "../FCTAPI.h"
namespace FCT
{
    VK_VertexShader::VK_VertexShader(VK_Context* ctx) : m_ctx(ctx)
        , m_module(VK_NULL_HANDLE)
    {
    }

    VK_VertexShader::~VK_VertexShader()
    {
        if (m_module != VK_NULL_HANDLE) {
            m_ctx->getDevice().destroyShaderModule(m_module);
        }
    }

    void VK_VertexShader::create()
    {
        createShaderModule();
        createPipelineStage();
    }

    void VK_VertexShader::bind()
    {

    }

    void VK_VertexShader::unbind()
    {

    }

    void VK_VertexShader::createShaderModule()
    {
        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.codeSize = m_code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(m_code.data());
        m_module = m_ctx->getDevice().createShaderModule(createInfo);
    }

    void VK_VertexShader::createPipelineStage()
    {
        m_stageInfo.stage = vk::ShaderStageFlagBits::eVertex;
        m_stageInfo.module = m_module;
        m_stageInfo.pName = "main";
    }
}
