//
// Created by Administrator on 2025/4/14.
//
#include "../FCTAPI.h"
namespace FCT
{
    VK_PassResource::VK_PassResource(VK_Context* ctx)
    {
        m_ctx = ctx;
    }

    void VK_PassResource::addConstBuffer(RHI::ConstBuffer* buffer)
    {
        if (buffer) {
            m_constBuffers.push_back(buffer);
            markAllDescriptorSetsDirty();
        }
    }

    void VK_PassResource::addSampler(Sampler* sampler, SamplerElement element)
    {
        if (sampler) {
            m_samplers.emplace_back(sampler, element);
            markAllDescriptorSetsDirty();
        }
    }

    void VK_PassResource::addTexture(Image* texture, TextureElement element)
    {
        if (texture) {
            m_textures.emplace_back(texture, element);
            markAllDescriptorSetsDirty();
        }
    }

    void VK_PassResource::markAllDescriptorSetsDirty()
    {
        uint32_t maxFrames = m_ctx->maxFrameInFlight();
        m_dirtyFlags.resize(maxFrames, true);
    }

    void VK_PassResource::markDescriptorSetDirty(uint32_t frameIdx)
    {
        if (frameIdx < m_dirtyFlags.size()) {
            m_dirtyFlags[frameIdx] = true;
        }
    }

   void VK_PassResource::create()
    {
        if (m_constBuffers.empty() && m_samplers.empty() && m_textures.empty()) {
            return;
        }

        vk::Device device = m_ctx->getDevice();
        uint32_t maxFrames = m_ctx->maxFrameInFlight();
        auto descriptorPool = static_cast<RHI::VK_DescriptorPool*>(m_ctx->getDescriptorPool(m_wnd));

        std::map<uint32_t, std::vector<vk::DescriptorSetLayoutBinding>> setBindings;

        for (auto* constBuffer : m_constBuffers) {
            auto* vkConstBuffer = static_cast<RHI::VK_ConstBuffer*>(constBuffer);
            auto uniformLayout = vkConstBuffer->layout();
            auto [setIndex, binding] = m_ctx->getGenerator()->getLayoutBinding(uniformLayout);

            vk::DescriptorSetLayoutBinding layoutBinding;
            layoutBinding.setBinding(binding);
            layoutBinding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
            layoutBinding.setDescriptorCount(1);
            layoutBinding.setStageFlags(ConvertToVkShaderStageFlags(uniformLayout.getShaderStages()));

            setBindings[setIndex].push_back(layoutBinding);
        }

        for (auto& [texture, element] : m_textures) {
            auto [setIndex, binding] = m_ctx->getGenerator()->getTextureBinding(element);

            vk::DescriptorSetLayoutBinding layoutBinding;
            layoutBinding.setBinding(binding);
            layoutBinding.setDescriptorType(vk::DescriptorType::eSampledImage);
            layoutBinding.setDescriptorCount(1);
            layoutBinding.setStageFlags(ConvertToVkShaderStageFlags(element.getShaderStages()));

            setBindings[setIndex].push_back(layoutBinding);
        }

        for (auto& [sampler, element] : m_samplers) {
            auto [setIndex, binding] = m_ctx->getGenerator()->getSamplerBinding(element);

            vk::DescriptorSetLayoutBinding layoutBinding;
            layoutBinding.setBinding(binding);
            layoutBinding.setDescriptorType(vk::DescriptorType::eSampler);
            layoutBinding.setDescriptorCount(1);
            layoutBinding.setStageFlags(ConvertToVkShaderStageFlags(element.getShaderStages()));

            setBindings[setIndex].push_back(layoutBinding);
        }

        if (setBindings.empty()) {
            return;
        }

        std::vector<vk::DescriptorSetLayout> layouts;
        m_descriptorSetLayouts.clear();

        uint32_t maxSetIndex = setBindings.rbegin()->first;
        layouts.resize(maxSetIndex + 1);
        m_descriptorSetLayouts.resize(maxSetIndex + 1);

        for (const auto& [setIndex, bindings] : setBindings) {
            vk::DescriptorSetLayoutCreateInfo layoutInfo;
            layoutInfo.setBindingCount(static_cast<uint32_t>(bindings.size()));
            layoutInfo.setPBindings(bindings.data());

            try {
                layouts[setIndex] = m_descriptorSetLayouts[setIndex] = device.createDescriptorSetLayout(layoutInfo);
            } catch (const vk::SystemError& e) {
                for (auto& layout : m_descriptorSetLayouts) {
                    if (layout) {
                        device.destroyDescriptorSetLayout(layout);
                        layout = nullptr;
                    }
                }
                throw std::runtime_error("Failed to create descriptor set layout: " + std::string(e.what()));
            }
        }

        m_descriptorSets.resize(maxFrames);
        m_dirtyFlags.resize(maxFrames, true);

        for (uint32_t frameIdx = 0; frameIdx < maxFrames; ++frameIdx) {
            vk::DescriptorSetAllocateInfo allocInfo;
            allocInfo.setDescriptorPool(descriptorPool->getPool());
            allocInfo.setDescriptorSetCount(static_cast<uint32_t>(layouts.size()));
            allocInfo.setPSetLayouts(layouts.data());

            try {
                m_descriptorSets[frameIdx] = device.allocateDescriptorSets(allocInfo);
                updateDescriptorSetsIfNeeded(frameIdx);
            } catch (const vk::SystemError& e) {
                throw std::runtime_error("Failed to allocate descriptor sets: " + std::string(e.what()));
            }
        }
    }
   void VK_PassResource::updateDescriptorSetsIfNeeded(uint32_t frameIdx) {
        if (frameIdx >= m_dirtyFlags.size() || !m_dirtyFlags[frameIdx]) {
            return;
        }

        vk::Device device = m_ctx->getDevice();

        // 预先分配足够的空间，避免扩容导致的指针失效
        std::vector<vk::WriteDescriptorSet> descriptorWrites;
        descriptorWrites.reserve(m_constBuffers.size() + m_textures.size() + m_samplers.size());

        std::vector<vk::DescriptorBufferInfo> bufferInfos;
        bufferInfos.reserve(m_constBuffers.size());

        std::vector<vk::DescriptorImageInfo> textureInfos;
        textureInfos.reserve(m_textures.size());

        std::vector<vk::DescriptorImageInfo> samplerInfos;
        samplerInfos.reserve(m_samplers.size());

        // 处理常量缓冲区
        for (size_t i = 0; i < m_constBuffers.size(); ++i) {
            auto* srcConstBuffer = m_constBuffers[i];
            auto* constBuffer = static_cast<RHI::VK_ConstBuffer*>(srcConstBuffer);
            auto uniformLayout = constBuffer->layout();
            auto [setIndex, binding] = m_ctx->getGenerator()->getLayoutBinding(uniformLayout);

            if (setIndex >= m_descriptorSets[frameIdx].size()) {
                continue;
            }

            // 添加缓冲区信息
            bufferInfos.push_back(constBuffer->currentBufferInfoWithoutUpdata());

            // 创建描述符写入
            vk::WriteDescriptorSet descriptorWrite;
            descriptorWrite.setDstSet(m_descriptorSets[frameIdx][setIndex]);
            descriptorWrite.setDstBinding(binding);
            descriptorWrite.setDstArrayElement(0);
            descriptorWrite.setDescriptorType(vk::DescriptorType::eUniformBuffer);
            descriptorWrite.setDescriptorCount(1);
            descriptorWrite.setPBufferInfo(&bufferInfos[bufferInfos.size() - 1]);

            descriptorWrites.push_back(descriptorWrite);
        }

        // 处理纹理
        for (size_t i = 0; i < m_textures.size(); ++i) {
            auto& [texture, element] = m_textures[i];
            auto* vkTexture = static_cast<RHI::VK_TextureView*>(texture->currentTextureView());
            auto [setIndex, binding] = m_ctx->getGenerator()->getTextureBinding(element);

            if (setIndex >= m_descriptorSets[frameIdx].size() || !vkTexture) {
                continue;
            }

            // 添加纹理信息
            vk::DescriptorImageInfo imageInfo;
            imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
            imageInfo.setImageView(vkTexture->view());
            imageInfo.setSampler(nullptr);
            textureInfos.push_back(imageInfo);

            // 创建描述符写入
            vk::WriteDescriptorSet descriptorWrite;
            descriptorWrite.setDstSet(m_descriptorSets[frameIdx][setIndex]);
            descriptorWrite.setDstBinding(binding);
            descriptorWrite.setDstArrayElement(0);
            descriptorWrite.setDescriptorType(vk::DescriptorType::eSampledImage);
            descriptorWrite.setDescriptorCount(1);
            descriptorWrite.setPImageInfo(&textureInfos[textureInfos.size() - 1]);

            descriptorWrites.push_back(descriptorWrite);
        }

        // 处理采样器
        for (size_t i = 0; i < m_samplers.size(); ++i) {
            auto& [sampler, element] = m_samplers[i];
            auto* vkSampler = static_cast<RHI::VK_Sampler*>(sampler);
            auto [setIndex, binding] = m_ctx->getGenerator()->getSamplerBinding(element);

            if (setIndex >= m_descriptorSets[frameIdx].size() || !vkSampler) {
                continue;
            }

            // 添加采样器信息
            vk::DescriptorImageInfo samplerInfo;
            samplerInfo.setSampler(vkSampler->getSampler());
            samplerInfo.setImageView(nullptr);
            samplerInfo.setImageLayout(vk::ImageLayout::eUndefined);
            samplerInfos.push_back(samplerInfo);

            // 创建描述符写入
            vk::WriteDescriptorSet descriptorWrite;
            descriptorWrite.setDstSet(m_descriptorSets[frameIdx][setIndex]);
            descriptorWrite.setDstBinding(binding);
            descriptorWrite.setDstArrayElement(0);
            descriptorWrite.setDescriptorType(vk::DescriptorType::eSampler);
            descriptorWrite.setDescriptorCount(1);
            descriptorWrite.setPImageInfo(&samplerInfos[samplerInfos.size() - 1]);

            descriptorWrites.push_back(descriptorWrite);
        }

        // 更新描述符集
        if (!descriptorWrites.empty()) {
            device.updateDescriptorSets(descriptorWrites, nullptr);
        }

        m_dirtyFlags[frameIdx] = false;
    }
    vk::DescriptorSet VK_PassResource::getDescriptorSet(uint32_t frameIdx, uint32_t setIndex)
    {
        updateDescriptorSetsIfNeeded(frameIdx);

        if (frameIdx < m_descriptorSets.size() && setIndex < m_descriptorSets[frameIdx].size()) {
            return m_descriptorSets[frameIdx][setIndex];
        }

        return nullptr;
    }

    void VK_PassResource::bind(RHI::CommandBuffer* cmdBuf, RHI::Pipeline* srcPipeline)
    {

        if (!srcPipeline || m_descriptorSets.empty()) {
            return;
        }

        uint32_t frameIdx = m_ctx->currentFrameIndex();

        updateDescriptorSetsIfNeeded(frameIdx);

        auto vkCmdBuf = static_cast<RHI::VK_CommandBuffer*>(cmdBuf);
        vk::CommandBuffer commandBuffer = vkCmdBuf->commandBuffer();

        auto pipeline = static_cast<RHI::VK_RasterizationPipeline*>(srcPipeline);
        vk::PipelineLayout pipelineLayout = pipeline->pipelineLayout();

        vk::PipelineBindPoint bindPoint = vk::PipelineBindPoint::eGraphics;

        if (!m_descriptorSets[frameIdx].empty()) {
            commandBuffer.bindDescriptorSets(
                bindPoint,
                pipelineLayout,
                0,
                static_cast<uint32_t>(m_descriptorSets[frameIdx].size()),
                m_descriptorSets[frameIdx].data(),
                0,
                nullptr
            );
        }
    }
}
