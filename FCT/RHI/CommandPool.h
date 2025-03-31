//
// Created by Administrator on 2025/3/31.
//
#include "../MutilThreadBase/RefCount.h"
#include "./CommandBuffer.h"
#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H
namespace FCT
{
    namespace RHI
    {
        class CommandPool : public RefCount
        {
        public:
            virtual void create() = 0;
            virtual CommandBuffer* createCommandBuffer() = 0;
        };
    }
}
#endif //COMMANDPOOL_H
