//
// Created by Administrator on 2025/3/23.
//

#include "./IPipelineResource.h"
#include "./Sampler.h"
#ifndef DEPTHSTENCILSTATE_H
#define DEPTHSTENCILSTATE_H

namespace FCT {
    namespace RHI
    {

        enum class StencilOp {
            Keep,
            Zero,
            Replace,
            IncrementClamp,
            DecrementClamp,
            Invert,
            IncrementWrap,
            DecrementWrap
        };

        struct StencilOpState {
            StencilOp failOp = StencilOp::Keep;
            StencilOp passOp = StencilOp::Keep;
            StencilOp depthFailOp = StencilOp::Keep;
            CompareOp compareOp = CompareOp::Always;
            uint32_t compareMask = 0xFF;
            uint32_t writeMask = 0xFF;
            uint32_t reference = 0;
        };

        class DepthStencilState : public IPipelineResource {
        public:
            virtual ~DepthStencilState() override = default;

            virtual void create() = 0;

            bool depthTestEnable() const { return m_depthTestEnable; }
            void depthTestEnable(bool enable) { m_depthTestEnable = enable; }

            bool depthWriteEnable() const { return m_depthWriteEnable; }
            void depthWriteEnable(bool enable) { m_depthWriteEnable = enable; }

            CompareOp depthCompareOp() const { return m_depthCompareOp; }
            void depthCompareOp(CompareOp op) { m_depthCompareOp = op; }

            bool depthBoundsTestEnable() const { return m_depthBoundsTestEnable; }
            void depthBoundsTestEnable(bool enable) { m_depthBoundsTestEnable = enable; }

            bool stencilTestEnable() const { return m_stencilTestEnable; }
            void stencilTestEnable(bool enable) { m_stencilTestEnable = enable; }

            const StencilOpState& front() const { return m_front; }
            void front(const StencilOpState& state) { m_front = state; }

            const StencilOpState& back() const { return m_back; }
            void back(const StencilOpState& state) { m_back = state; }

            float minDepthBounds() const { return m_minDepthBounds; }
            void minDepthBounds(float value) { m_minDepthBounds = value; }

            float maxDepthBounds() const { return m_maxDepthBounds; }
            void maxDepthBounds(float value) { m_maxDepthBounds = value; }

            PipelineResourceType getType() const override { return PipelineResourceType::DepthStencilState; }

        protected:
            bool m_depthTestEnable = true;
            bool m_depthWriteEnable = true;
            CompareOp m_depthCompareOp = CompareOp::LessOrEqual;
            bool m_depthBoundsTestEnable = false;
            bool m_stencilTestEnable = false;
            StencilOpState m_front;
            StencilOpState m_back;
            float m_minDepthBounds = 0.0f;
            float m_maxDepthBounds = 1.0f;
        };
    }
}

#endif //DEPTHSTENCILSTATE_H