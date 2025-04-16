//
// Created by Administrator on 2025/3/23.
//

#ifndef VK_DEPTHSTENCILSTATE_H
#define VK_DEPTHSTENCILSTATE_H

#include "./DepthStencilState.h"
#include "../Context/VK_Context.h"

namespace FCT {
    namespace RHI
    {
        class VK_DepthStencilState : public DepthStencilState {
        public:
            explicit VK_DepthStencilState(VK_Context* ctx);
            ~VK_DepthStencilState() override = default;

            void create() override;

            vk::PipelineDepthStencilStateCreateInfo& getCreateInfo() { return m_createInfo; }

        private:
            VK_Context* m_ctx;
            vk::PipelineDepthStencilStateCreateInfo m_createInfo;
            vk::StencilOpState m_frontStencilState;
            vk::StencilOpState m_backStencilState;

            vk::CompareOp convertCompareOp(CompareOp op) const;
            vk::StencilOp convertStencilOp(StencilOp op) const;
            vk::StencilOpState convertStencilOpState(const StencilOpState& state) const;
        };
    }
}

#endif //VK_DEPTHSTENCILSTATE_H