//
// Created by Administrator on 2025/4/1.
//
#include "../MutilThreadBase/RefCount.h"
#ifndef SEMAPHORE_H
#define SEMAPHORE_H

namespace FCT
{
    namespace RHI
    {
        class Semaphore : public RefCount
        {
        public:
            //实在需要reset可以 调用create,但是不推荐
            virtual void create() = 0;

        protected:

        };
    }
}


#endif //SEMAPHORE_H
