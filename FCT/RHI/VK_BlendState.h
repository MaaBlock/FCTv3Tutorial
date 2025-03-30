//
// Created by Administrator on 2025/3/23.
//
#include "../ThirdParty.h"
#include "./BlendState.h"
#ifndef VK_BLENDSTATE_H
#define VK_BLENDSTATE_H

namespace FCT
{
    class VK_Context;
    class VK_BlendState : public BlendState  {
    public:
        VK_BlendState(VK_Context* ctx);
        void create() override;
        void bind() override;
        void unbind() override;
    private:
        VK_Context* m_ctx;
        vk::PipelineColorBlendAttachmentState m_attachmentState;
        vk::PipelineColorBlendStateCreateInfo m_createInfo;

        vk::BlendFactor convertBlendFactor(BlendFactor factor) const;
        vk::BlendOp convertBlendOp(BlendOp op) const;
        vk::LogicOp convertLogicOp(LogicOp op) const;
    };
}

#endif //VK_BLENDSTATE_H