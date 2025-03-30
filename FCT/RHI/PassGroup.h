//
// Created by Administrator on 2025/3/25.
//
#include "../MutilThreadBase/RefCount.h"
#include "../Context/IRenderTarget.h"
#include "./Pass.h"
#ifndef FCT_RHI_PASSGROUP_H
#define FCT_RHI_PASSGROUP_H
namespace FCT
{
    namespace RHI
    {
        class PassGroup
        {
        public:
            PassGroup()
            {

            }
            virtual ~PassGroup()
            {

            }
            virtual void create() = 0;
            virtual void addTarget(IRenderTarget* target)
            {
                m_targets.push_back(target);
            }
            void addPass(Pass* pass)
            {
                m_passes.push_back(pass);
            }
        protected:
            std::vector<PassGroup*> m_prevPassGroup;
            std::vector<PassGroup*> m_nextPassGroup;
            std::vector<IRenderTarget*> m_targets;
            std::vector<Pass*> m_passes;
        };
    }
}
#endif //FCT_RHI_PASSGROUP_H
