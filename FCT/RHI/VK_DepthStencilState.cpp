//
// Created by Administrator on 2025/3/23.
//

#include "../FCTAPI.h"

namespace FCT {
    namespace RHI
    {
        VK_DepthStencilState::VK_DepthStencilState(VK_Context* ctx) : m_frontStencilState{}, m_backStencilState{}, m_createInfo{}
        {
            m_ctx = ctx;
        }

        void VK_DepthStencilState::create()
        {
            m_frontStencilState = convertStencilOpState(m_front);
            m_backStencilState = convertStencilOpState(m_back);

            m_createInfo = vk::PipelineDepthStencilStateCreateInfo()
                .setDepthTestEnable(m_depthTestEnable)
                .setDepthWriteEnable(m_depthWriteEnable)
                .setDepthCompareOp(convertCompareOp(m_depthCompareOp))
                .setDepthBoundsTestEnable(m_depthBoundsTestEnable)
                .setStencilTestEnable(m_stencilTestEnable)
                .setFront(m_frontStencilState)
                .setBack(m_backStencilState)
                .setMinDepthBounds(m_minDepthBounds)
                .setMaxDepthBounds(m_maxDepthBounds);
        }

        vk::CompareOp VK_DepthStencilState::convertCompareOp(CompareOp op) const
        {
            switch (op)
            {
            case CompareOp::Never: return vk::CompareOp::eNever;
            case CompareOp::Less: return vk::CompareOp::eLess;
            case CompareOp::Equal: return vk::CompareOp::eEqual;
            case CompareOp::LessOrEqual: return vk::CompareOp::eLessOrEqual;
            case CompareOp::Greater: return vk::CompareOp::eGreater;
            case CompareOp::NotEqual: return vk::CompareOp::eNotEqual;
            case CompareOp::GreaterOrEqual: return vk::CompareOp::eGreaterOrEqual;
            case CompareOp::Always: return vk::CompareOp::eAlways;
            default: return vk::CompareOp::eAlways;
            }
        }

        vk::StencilOp VK_DepthStencilState::convertStencilOp(StencilOp op) const
        {
            switch (op)
            {
            case StencilOp::Keep: return vk::StencilOp::eKeep;
            case StencilOp::Zero: return vk::StencilOp::eZero;
            case StencilOp::Replace: return vk::StencilOp::eReplace;
            case StencilOp::IncrementClamp: return vk::StencilOp::eIncrementAndClamp;
            case StencilOp::DecrementClamp: return vk::StencilOp::eDecrementAndClamp;
            case StencilOp::Invert: return vk::StencilOp::eInvert;
            case StencilOp::IncrementWrap: return vk::StencilOp::eIncrementAndWrap;
            case StencilOp::DecrementWrap: return vk::StencilOp::eDecrementAndWrap;
            default: return vk::StencilOp::eKeep;
            }
        }

        vk::StencilOpState VK_DepthStencilState::convertStencilOpState(const StencilOpState& state) const
        {
            return vk::StencilOpState()
                .setFailOp(convertStencilOp(state.failOp))
                .setPassOp(convertStencilOp(state.passOp))
                .setDepthFailOp(convertStencilOp(state.depthFailOp))
                .setCompareOp(convertCompareOp(state.compareOp))
                .setCompareMask(state.compareMask)
                .setWriteMask(state.writeMask)
                .setReference(state.reference);
        }
    }
}