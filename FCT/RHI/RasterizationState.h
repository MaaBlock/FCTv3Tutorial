//
// Created by Administrator on 2025/3/23.
//
#include "./IPipelineResource.h"
#ifndef RASTERIZATIONSTATE_H
#define RASTERIZATIONSTATE_H

namespace FCT
{
    enum class PolygonMode {
        Fill,
        Line,
        Point
    };

    enum class CullMode {
        None,
        Front,
        Back,
        FrontAndBack
    };

    enum class FrontFace {
        Clockwise,
        CounterClockwise
    };

    class RasterizationState : public IPipelineResource {
    public:
        virtual ~RasterizationState() = default;

        virtual void create() = 0;

        PolygonMode polygonMode() const { return m_polygonMode; }
        void polygonMode(PolygonMode mode) { m_polygonMode = mode; }

        CullMode cullMode() const { return m_cullMode; }
        void cullMode(CullMode mode) { m_cullMode = mode; }

        FrontFace frontFace() const { return m_frontFace; }
        void frontFace(FrontFace face) { m_frontFace = face; }

        float lineWidth() const { return m_lineWidth; }
        void lineWidth(float width) { m_lineWidth = width; }

        bool depthClampEnable() const { return m_depthClampEnable; }
        void depthClampEnable(bool enable) { m_depthClampEnable = enable; }

        bool rasterizerDiscardEnable() const { return m_rasterizerDiscardEnable; }
        void rasterizerDiscardEnable(bool enable) { m_rasterizerDiscardEnable = enable; }

        bool depthBiasEnable() const { return m_depthBiasEnable; }
        void depthBiasEnable(bool enable) { m_depthBiasEnable = enable; }

        float depthBiasConstantFactor() const { return m_depthBiasConstantFactor; }
        void depthBiasConstantFactor(float factor) { m_depthBiasConstantFactor = factor; }

        float depthBiasClamp() const { return m_depthBiasClamp; }
        void depthBiasClamp(float clamp) { m_depthBiasClamp = clamp; }

        float depthBiasSlopeFactor() const { return m_depthBiasSlopeFactor; }
        void depthBiasSlopeFactor(float factor) { m_depthBiasSlopeFactor = factor; }

        bool multisampleEnable() const { return m_multisampleEnable; }
        void multisampleEnable(bool enable) { m_multisampleEnable = enable; }

        uint32_t sampleCount() const { return m_sampleCount; }
        void sampleCount(uint32_t count) { m_sampleCount = count; }

        float minSampleShading() const { return m_minSampleShading; }
        void minSampleShading(float value) { m_minSampleShading = value; }

        uint32_t sampleMask() const { return m_sampleMask; }
        void sampleMask(uint32_t mask) { m_sampleMask = mask; }

        bool alphaToCoverageEnable() const { return m_alphaToCoverageEnable; }
        void alphaToCoverageEnable(bool enable) { m_alphaToCoverageEnable = enable; }

        bool alphaToOneEnable() const { return m_alphaToOneEnable; }
        void alphaToOneEnable(bool enable) { m_alphaToOneEnable = enable; }

        PipelineResourceType getType() const override { return PipelineResourceType::RasterizationState; }

    protected:
        PolygonMode m_polygonMode = PolygonMode::Fill;
        CullMode m_cullMode = CullMode::None;
        FrontFace m_frontFace = FrontFace::CounterClockwise;
        float m_lineWidth = 1.0f;
        bool m_depthClampEnable = false;
        bool m_rasterizerDiscardEnable = false;
        bool m_depthBiasEnable = false;
        float m_depthBiasConstantFactor = 0.0f;
        float m_depthBiasClamp = 0.0f;
        float m_depthBiasSlopeFactor = 0.0f;
        bool m_multisampleEnable = false;
        uint32_t m_sampleCount = 1;
        float m_minSampleShading = 1.0f;
        uint32_t m_sampleMask = 0xFFFFFFFF;
        bool m_alphaToCoverageEnable = false;
        bool m_alphaToOneEnable = false;
    };
}

#endif //RASTERIZATIONSTATE_H