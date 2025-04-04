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
            virtual void create() = 0;
        protected:

        };
    }
}


#endif //SEMAPHORE_H
