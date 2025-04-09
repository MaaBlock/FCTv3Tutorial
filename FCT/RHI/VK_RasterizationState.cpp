//
// Created by Administrator on 2025/3/23.
//

#include "../FCTAPI.h"
namespace FCT
{
    VK_RasterizationState::VK_RasterizationState(VK_Context* ctx)
    {
        m_ctx = ctx;
    }

    void VK_RasterizationState::create()
    {
        m_rasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo()
            .setDepthClampEnable(depthClampEnable())
            .setRasterizerDiscardEnable(rasterizerDiscardEnable())
            .setPolygonMode(convertPolygonMode(polygonMode()))
            .setCullMode(convertCullMode(cullMode()))
            .setFrontFace(convertFrontFace(frontFace()))
            .setDepthBiasEnable(depthBiasEnable())
            .setDepthBiasConstantFactor(depthBiasConstantFactor())
            .setDepthBiasClamp(depthBiasClamp())
            .setDepthBiasSlopeFactor(depthBiasSlopeFactor())
            .setLineWidth(lineWidth());
        m_multisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo()
            .setRasterizationSamples(static_cast<vk::SampleCountFlagBits>(m_sampleCount))
            .setSampleShadingEnable(m_minSampleShading < 1.0f)
            .setMinSampleShading(m_minSampleShading)
            .setPSampleMask(&m_sampleMask)
            .setAlphaToCoverageEnable(m_alphaToCoverageEnable)
            .setAlphaToOneEnable(m_alphaToOneEnable);
    }
}
