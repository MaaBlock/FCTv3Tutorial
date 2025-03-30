//
// Created by Administrator on 2025/3/23.
//

#include "../FCTAPI.h"
namespace FCT
{
    VK_BlendState::VK_BlendState(VK_Context* ctx) : m_attachmentState{} , m_createInfo{}
    {
        m_ctx = ctx;
    }

    void VK_BlendState::create()
    {
        m_attachmentState.blendEnable = m_blendEnable;
        m_attachmentState.srcColorBlendFactor = convertBlendFactor(m_srcColorBlendFactor);
        m_attachmentState.dstColorBlendFactor = convertBlendFactor(m_dstColorBlendFactor);
        m_attachmentState.colorBlendOp = convertBlendOp(m_colorBlendOp);
        m_attachmentState.srcAlphaBlendFactor = convertBlendFactor(m_srcAlphaBlendFactor);
        m_attachmentState.dstAlphaBlendFactor = convertBlendFactor(m_dstAlphaBlendFactor);
        m_attachmentState.alphaBlendOp = convertBlendOp(m_alphaBlendOp);
        m_attachmentState.colorWriteMask = static_cast<vk::ColorComponentFlags>(m_colorWriteMask);

        m_createInfo = vk::PipelineColorBlendStateCreateInfo()
            .setLogicOpEnable(m_logicOpEnable)
            .setLogicOp(convertLogicOp(m_logicOp))
            .setAttachmentCount(1)
            .setPAttachments(&m_attachmentState)
            .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
    }



    void VK_BlendState::bind()
    {

    }

    void VK_BlendState::unbind()
    {

    }


      vk::BlendFactor VK_BlendState::convertBlendFactor(BlendFactor factor) const
    {
        switch (factor)
        {
            case BlendFactor::Zero: return vk::BlendFactor::eZero;
            case BlendFactor::One: return vk::BlendFactor::eOne;
            case BlendFactor::SrcColor: return vk::BlendFactor::eSrcColor;
            case BlendFactor::OneMinusSrcColor: return vk::BlendFactor::eOneMinusSrcColor;
            case BlendFactor::DstColor: return vk::BlendFactor::eDstColor;
            case BlendFactor::OneMinusDstColor: return vk::BlendFactor::eOneMinusDstColor;
            case BlendFactor::SrcAlpha: return vk::BlendFactor::eSrcAlpha;
            case BlendFactor::OneMinusSrcAlpha: return vk::BlendFactor::eOneMinusSrcAlpha;
            case BlendFactor::DstAlpha: return vk::BlendFactor::eDstAlpha;
            case BlendFactor::OneMinusDstAlpha: return vk::BlendFactor::eOneMinusDstAlpha;
            case BlendFactor::ConstantColor: return vk::BlendFactor::eConstantColor;
            case BlendFactor::OneMinusConstantColor: return vk::BlendFactor::eOneMinusConstantColor;
            case BlendFactor::Src1Color: return vk::BlendFactor::eSrc1Color;
            case BlendFactor::OneMinusSrc1Color: return vk::BlendFactor::eOneMinusSrc1Color;
            case BlendFactor::Src1Alpha: return vk::BlendFactor::eSrc1Alpha;
            case BlendFactor::OneMinusSrc1Alpha: return vk::BlendFactor::eOneMinusSrc1Alpha;
            default: return vk::BlendFactor::eOne;
        }
    }

    vk::BlendOp VK_BlendState::convertBlendOp(BlendOp op) const
    {
        switch (op)
        {
            case BlendOp::Add: return vk::BlendOp::eAdd;
            case BlendOp::Subtract: return vk::BlendOp::eSubtract;
            case BlendOp::ReverseSubtract: return vk::BlendOp::eReverseSubtract;
            case BlendOp::Min: return vk::BlendOp::eMin;
            case BlendOp::Max: return vk::BlendOp::eMax;
            default: return vk::BlendOp::eAdd;
        }
    }

    vk::LogicOp VK_BlendState::convertLogicOp(LogicOp op) const
    {
        switch (op)
        {
            case LogicOp::Clear: return vk::LogicOp::eClear;
            case LogicOp::And: return vk::LogicOp::eAnd;
            case LogicOp::AndReverse: return vk::LogicOp::eAndReverse;
            case LogicOp::Copy: return vk::LogicOp::eCopy;
            case LogicOp::AndInverted: return vk::LogicOp::eAndInverted;
            case LogicOp::NoOp: return vk::LogicOp::eNoOp;
            case LogicOp::Xor: return vk::LogicOp::eXor;
            case LogicOp::Or: return vk::LogicOp::eOr;
            case LogicOp::Nor: return vk::LogicOp::eNor;
            case LogicOp::Equivalent: return vk::LogicOp::eEquivalent;
            case LogicOp::Invert: return vk::LogicOp::eInvert;
            case LogicOp::OrReverse: return vk::LogicOp::eOrReverse;
            case LogicOp::CopyInverted: return vk::LogicOp::eCopyInverted;
            case LogicOp::OrInverted: return vk::LogicOp::eOrInverted;
            case LogicOp::Nand: return vk::LogicOp::eNand;
            case LogicOp::Set: return vk::LogicOp::eSet;
            default: return vk::LogicOp::eCopy;
        }
    }
}
