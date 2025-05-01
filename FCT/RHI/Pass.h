//
// Created by Administrator on 2025/3/25.
//
#include "../Base/Flags.h"
#include "../MutilThreadBase/RefCount.h"
#include "../Context/Image.h"
#ifndef FCT_RHI_PASS_H
#define FCT_RHI_PASS_H
namespace FCT
{
    enum class ClearType : uint32_t
    {
        color = 0x1,
        depth = 0x2,
        stencil = 0x4,
        depthStencil = depth | stencil
    };
    FCT_DECLARE_FLAGS(ClearType);
    struct PassClearValue
    {
        ClearTypes types;
        Vec4 color;
        float depth;
        uint8_t stencil;
    };
    namespace RHI
    {
        class PassGroup;
        class Pass : public RefCount
        {
        public:
            Pass()
            {
                m_group = nullptr;
                m_depthStencil = nullptr;
            }
            void bindTarget(uint32_t index, FCT::Image* target)
            {
                m_renderTargets[index] = target;
            }
            void bindTexture(uint32_t index, FCT::Image* texture)
            {
                m_textures[index] = texture;
            }
            void depthStencil(FCT::Image* depthStencil)
            {
                m_depthStencil = depthStencil;
            }
            FCT::Image* depthStencil() const
            {
                return m_depthStencil;
            }
            virtual void create(PassGroup* group) = 0;//由PassGroup 调用
            void enableClear(ClearTypes type,Vec4 color,float depth = 1.0f, uint8_t stencil = 0)
            {
                m_clearValue.types = type;
                m_clearValue.color = color;
                m_clearValue.depth = depth;
                m_clearValue.stencil = stencil;
            }
            void enableClear(PassClearValue clearValue)
            {
                m_clearValue = clearValue;
            }
            auto passGroup() const
            {
                return m_group;
            }
            auto& renderTargets()
            {
                return m_renderTargets;
            }
            virtual void beginSubmit(CommandBuffer* cmdBuf) = 0;
            virtual void endSubmit() = 0;
            uint32_t index() const
            {
                return m_index;
            }
            void index(uint32_t index)
            {
                m_index = index;
            }
            PassGroup* group() const
            {
                return m_group;
            }
        protected:
            FCT::Image* m_depthStencil;
            PassGroup* m_group;
            uint32_t m_index;
            //傻逼Vulkan，这tm也要自己写
            std::map<uint32_t,FCT::Image*> m_renderTargets;
            std::map<uint32_t,FCT::Image*> m_textures;
            PassClearValue m_clearValue;
        };
    }
}
#endif //FCT_RHI_PASS_H
