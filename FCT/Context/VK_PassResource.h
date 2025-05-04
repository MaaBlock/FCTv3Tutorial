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
        void addSampler(Sampler* sampler, SamplerElement element) override;
        void addTexture(Image* texture, TextureElement element) override;
        void setTexture(Image* texture, TextureElement element) override;
        void markAllDescriptorSetsDirty();
        void markDescriptorSetDirty(uint32_t frameIdx);
        bool createDescriptorSetsAndLayouts(uint32_t frameIdx, std::vector<vk::DescriptorSetLayout>& outLayouts,
                                            std::vector<vk::DescriptorSet>& outDescriptorSets);
        void create() override;
        void updateDescriptorSetsIfNeeded(uint32_t frameIdx);
        vk::DescriptorSet getDescriptorSet(uint32_t frameIdx, uint32_t setIndex);
        void markAllDescriptorSetsNeedRecreate();
        bool recreateDescriptorSetsIfNeeded(uint32_t frameIdx);
        void update() override;
        void bind(RHI::CommandBuffer* cmdBuf, RHI::Pipeline* pipeline) override;
    protected:
        VK_Context* m_ctx;
        std::vector<std::vector<vk::DescriptorSet>> m_descriptorSets;
        std::vector<uint8_t> m_dirtyFlags;
        std::vector<uint8_t> m_needRecreate;
        std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
    };
}
#endif //VK_PASSRESOURCE_H
