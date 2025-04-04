//
// Created by Administrator on 2025/4/2.
//
#include "../MutilThreadBase/RefCount.h"
#ifndef FENCE_H
#define FENCE_H
namespace FCT
{
    namespace RHI
    {
        class Fence : public RefCount
        {
        public:
            Fence()
            {
                m_signaled = false;
            }
            void initSignal(bool signal = true)
            {
                m_signaled = signal;
            }
            virtual void reset() = 0;
            virtual void create() = 0;
            virtual void waitFor() = 0;
            virtual void createSignaled()
            {
                initSignal(true);
                create();
            }
        protected:
            bool m_signaled;
        };
    }
}
#endif //FENCE_H
