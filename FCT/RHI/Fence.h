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
        class Fence : public RefCount,public DeletableTrait<Fence>
        {
        public:
            Fence() : RefCount(0)
            {
                m_signaled = false;
            }
            virtual ~Fence() {}
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
            virtual void destroy() = 0;
            void deleteThis() override {
                if (!this->applyDeleter(this)) {
                    FCT_DELETE(this);
                }
            }
        protected:
            bool m_signaled;
        };
    }
}
#endif //FENCE_H
