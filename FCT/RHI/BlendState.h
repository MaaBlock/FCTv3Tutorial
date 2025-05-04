//
// Created by Administrator on 2025/3/23.
//

#include "./IPipelineResource.h"

#ifndef BLENDSTATE_H
#define BLENDSTATE_H

namespace FCT {
    enum class BlendFactor {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        ConstantColor,
        OneMinusConstantColor,
        Src1Color,
        OneMinusSrc1Color,
        Src1Alpha,
        OneMinusSrc1Alpha
    };

    enum class BlendOp {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    enum class LogicOp {
        Clear,
        And,
        AndReverse,
        Copy,
        AndInverted,
        NoOp,
        Xor,
        Or,
        Nor,
        Equivalent,
        Invert,
        OrReverse,
        CopyInverted,
        OrInverted,
        Nand,
        Set
    };

    class BlendState : public IPipelineResource {
    public:
        virtual ~BlendState() override = default;

        virtual void create() = 0;

        bool blendEnable() const { return m_blendEnable; }
        void blendEnable(bool enable) { m_blendEnable = enable; }

        BlendFactor srcColorBlendFactor() const { return m_srcColorBlendFactor; }
        void srcColorBlendFactor(BlendFactor factor) { m_srcColorBlendFactor = factor; }

        BlendFactor dstColorBlendFactor() const { return m_dstColorBlendFactor; }
        void dstColorBlendFactor(BlendFactor factor) { m_dstColorBlendFactor = factor; }

        BlendOp colorBlendOp() const { return m_colorBlendOp; }
        void colorBlendOp(BlendOp op) { m_colorBlendOp = op; }

        BlendFactor srcAlphaBlendFactor() const { return m_srcAlphaBlendFactor; }
        void srcAlphaBlendFactor(BlendFactor factor) { m_srcAlphaBlendFactor = factor; }

        BlendFactor dstAlphaBlendFactor() const { return m_dstAlphaBlendFactor; }
        void dstAlphaBlendFactor(BlendFactor factor) { m_dstAlphaBlendFactor = factor; }

        BlendOp alphaBlendOp() const { return m_alphaBlendOp; }
        void alphaBlendOp(BlendOp op) { m_alphaBlendOp = op; }

        uint8_t colorWriteMask() const { return m_colorWriteMask; }
        void colorWriteMask(uint8_t mask) { m_colorWriteMask = mask; }

        bool logicOpEnable() const { return m_logicOpEnable; }
        void logicOpEnable(bool enable) { m_logicOpEnable = enable; }

        LogicOp logicOp() const { return m_logicOp; }
        void logicOp(LogicOp op) { m_logicOp = op; }

        PipelineResourceType getType() const override { return PipelineResourceType::BlendState; }

    protected:
        bool m_blendEnable = true;
        BlendFactor m_srcColorBlendFactor = BlendFactor::SrcAlpha;
        BlendFactor m_dstColorBlendFactor = BlendFactor::OneMinusSrcAlpha;
        BlendOp m_colorBlendOp = BlendOp::Add;
        BlendFactor m_srcAlphaBlendFactor = BlendFactor::One;
        BlendFactor m_dstAlphaBlendFactor = BlendFactor::OneMinusSrcAlpha;
        BlendOp m_alphaBlendOp = BlendOp::Add;
        uint8_t m_colorWriteMask = 0xF;
        bool m_logicOpEnable = false;
        LogicOp m_logicOp = LogicOp::Copy;
    };
}

#endif //BLENDSTATE_H