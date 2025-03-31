//
// Created by Administrator on 2025/3/25.
//
#include "../MutilThreadBase/RefCount.h"
#include "../Context/Image.h"
#ifndef FCT_RHI_PASS_H
#define FCT_RHI_PASS_H
namespace FCT
{
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
            /*
            void addTarget(FCT::Image* target)
            {
                m_renderTargets.push_back(target);
            }*/
            /*
            void addTargets(std::vector<FCT::Image*> targets)
            {
                m_renderTargets.insert(m_renderTargets.end(), targets.begin(), targets.end());
            }*/
            /*
            void addTexture(FCT::Image* texture)
            {
                m_textures.push_back(texture);
            }*/
            void bindTexture(uint32_t index, FCT::Image* texture)
            {
                m_textures[index] = texture;
            }
            void depthStencil(FCT::Image* depthStencil)
            {
                m_depthStencil = depthStencil;
            }
            virtual void create(PassGroup* group) = 0;//由PassGroup 调用
            /*auto& renderTargets()
            {
                return m_renderTargets;
            }*/
            auto passGroup() const
            {
                return m_group;
            }
            auto& renderTargets()
            {
                return m_renderTargets;
            }
        protected:
            //std::vector<FCT::Image*> m_renderTargets;
            //std::vector<FCT::Image*> m_textures;
            FCT::Image* m_depthStencil;
            PassGroup* m_group;

            //傻逼Vulkan，这tm也要自己写
            std::map<uint32_t,FCT::Image*> m_renderTargets;
            std::map<uint32_t,FCT::Image*> m_textures;
        };
    }
}
#endif //FCT_RHI_PASS_H
