//
// Created by Administrator on 2025/4/1.
//
#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include "../MutilThreadBase/RefCount.h"
namespace FCT
{
    namespace RHI
    {
        class Semaphore : public FCT::RefCount,public FCT::DeletableTrait<Semaphore>
        {
        public:
            Semaphore() : FCT::RefCount(0)
            {

            }
            virtual ~Semaphore() = default;
            //实在需要reset可以 调用create,但是不推荐
            virtual void create() = 0;
            virtual void destroy() = 0;
            void deleteThis() override {
                if (!this->applyDeleter(this)) {
                    FCT_DELETE(this);
                }
            }
        protected:

        };
    }
}


#endif //SEMAPHORE_H
