//
// Created by Administrator on 2025/4/14.
//
#include "./PassResource.h"
#ifndef VK_PASSRESOURCE_H
#define VK_PASSRESOURCE_H
namespace FCT
{
    class VK_PassResource : public PassResource{
    public:
        VK_PassResource(VK_Context* ctx);
        void addConstBuffer(RHI::ConstBuffer* buffer) override;
        void markAllDescriptorSetsDirty();
        void markDescriptorSetDirty(uint32_t frameIdx);
        void create() override;
        void updateDescriptorSetsIfNeeded(uint32_t frameIdx);
        vk::DescriptorSet getDescriptorSet(uint32_t frameIdx, uint32_t setIndex);
        void bind(RHI::CommandBuffer* cmdBuf) override;
        void update() override
        {
            
        }
    protected:
        VK_Context* m_ctx;
        std::vector<std::vector<vk::DescriptorSet>> m_descriptorSets;
        std::vector<bool> m_dirtyFlags;
    };
}
#endif //VK_PASSRESOURCE_H
