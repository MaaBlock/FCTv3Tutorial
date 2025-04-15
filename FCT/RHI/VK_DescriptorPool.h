//
// Created by Administrator on 2025/4/14.
//
#include "DescriptorPool.h"
#ifndef VK_DESCRIPTORPOOL_H
#define VK_DESCRIPTORPOOL_H
namespace FCT
{
    namespace RHI
    {
        class VK_DescriptorPool : public DescriptorPool{
        public:
            VK_DescriptorPool(VK_Context* ctx);
            void create();
            vk::DescriptorPool& getPool() { return m_descriptorPool; }
        protected:
            VK_Context* m_ctx;
            vk::DescriptorPool m_descriptorPool;
        };

    }
}



#endif //VK_DESCRIPTORPOOL_H
