//
// Created by Administrator on 2025/3/23.
//
#include "../ThirdParty.h"
#include "./RasterizationState.h"
#ifndef VK_RASTERIZATIONSTATE_H
#define VK_RASTERIZATIONSTATE_H
namespace FCT
{
    class VK_Context;
    class VK_RasterizationState : public RasterizationState {
    public:
        VK_RasterizationState(VK_Context* ctx);
        void bind() override;
        void unbind() override;
        void create() override;
    private:
        VK_Context* m_ctx;
        vk::PipelineRasterizationStateCreateInfo m_rasterizationStateCreateInfo;
        vk::PipelineMultisampleStateCreateInfo m_multisampleStateCreateInfo;
        vk::PolygonMode convertPolygonMode(PolygonMode mode) const
        {
            switch (mode) {
            case PolygonMode::Fill:  return vk::PolygonMode::eFill;
            case PolygonMode::Line:  return vk::PolygonMode::eLine;
            case PolygonMode::Point: return vk::PolygonMode::ePoint;
            default: throw std::runtime_error("Unsupported PolygonMode");
            }
        }

        vk::CullModeFlags convertCullMode(CullMode mode) const
        {
            switch (mode) {
            case CullMode::None:         return vk::CullModeFlagBits::eNone;
            case CullMode::Front:        return vk::CullModeFlagBits::eFront;
            case CullMode::Back:         return vk::CullModeFlagBits::eBack;
            case CullMode::FrontAndBack: return vk::CullModeFlagBits::eFrontAndBack;
            default: throw std::runtime_error("Unsupported CullMode");
            }
        }

        vk::FrontFace convertFrontFace(FrontFace face) const
        {
            switch (face) {
            case FrontFace::Clockwise:        return vk::FrontFace::eClockwise;
            case FrontFace::CounterClockwise: return vk::FrontFace::eCounterClockwise;
            default: throw std::runtime_error("Unsupported FrontFace");
            }
        }
    };
}



#endif //VK_RASTERIZATIONSTATE_H
