//
// Created by Administrator on 2025/3/25.
//
#include "../MutilThreadBase/RefCount.h"
#include "../Context/IRenderTarget.h"
#include "./CommandBuffer.h"
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
            virtual void beginSubmit(CommandBuffer* cmdBuf) = 0;
            virtual void endSubmit(CommandBuffer* cmdBuf) = 0;
            virtual void create() = 0;
            void addPass(Pass* pass)
            {
                m_passes.push_back(pass);
            }
        protected:
            std::vector<PassGroup*> m_prevPassGroup;
            std::vector<PassGroup*> m_nextPassGroup;
            std::vector<Pass*> m_passes;
        };
    }
}
#endif //FCT_RHI_PASSGROUP_H
