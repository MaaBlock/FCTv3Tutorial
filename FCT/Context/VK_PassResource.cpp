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
        if (m_constBuffers.empty()) {
            return;
        }

        vk::Device device = m_ctx->getDevice();
        uint32_t maxFrames = m_ctx->maxFrameInFlight();

        auto pipeline = static_cast<RHI::VK_RasterizationPipeline*>(m_pipeline);
        auto descriptorPool = static_cast<RHI::VK_DescriptorPool*>(m_ctx->getDescriptorPool(m_wnd));

        const uint32_t maxSets = 4;

        std::vector<vk::DescriptorSetLayout> layouts = pipeline->descriptorSetLayouts();

        if (layouts.empty()) {
            return;
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

    void VK_PassResource::updateDescriptorSetsIfNeeded(uint32_t frameIdx)
    {
        if (frameIdx >= m_dirtyFlags.size() || !m_dirtyFlags[frameIdx]) {
            return;
        }

        vk::Device device = m_ctx->getDevice();
        std::vector<vk::WriteDescriptorSet> descriptorWrites;
        std::vector<vk::DescriptorBufferInfo> bufferInfos;

        for (size_t i = 0; i < m_constBuffers.size(); ++i) {
            auto* srcConstBuffer = m_constBuffers[i];
            auto* constBuffer = static_cast<RHI::VK_ConstBuffer*>(srcConstBuffer);
            auto uniformLayout = constBuffer->layout();
            auto [setIndex, binding] = m_ctx->getGenerator()->getLayoutBinding(uniformLayout);

            if (setIndex >= m_descriptorSets[frameIdx].size()) {
                continue;
            }

            bufferInfos.push_back(constBuffer->currentBufferInfoWithoutUpdata());

            vk::WriteDescriptorSet descriptorWrite;
            descriptorWrite.setDstSet(m_descriptorSets[frameIdx][setIndex]);
            descriptorWrite.setDstBinding(binding);
            descriptorWrite.setDstArrayElement(0);
            descriptorWrite.setDescriptorType(vk::DescriptorType::eUniformBuffer);
            descriptorWrite.setDescriptorCount(1);
            descriptorWrite.setPBufferInfo(&bufferInfos.back());

            descriptorWrites.push_back(descriptorWrite);
        }

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

    void VK_PassResource::bind(RHI::CommandBuffer* cmdBuf)
    {
        if (!m_pipeline || m_descriptorSets.empty()) {
            return;
        }

        uint32_t frameIdx = m_ctx->currentFrameIndex();

        updateDescriptorSetsIfNeeded(frameIdx);

        auto vkCmdBuf = static_cast<RHI::VK_CommandBuffer*>(cmdBuf);
        vk::CommandBuffer commandBuffer = vkCmdBuf->commandBuffer();

        auto pipeline = static_cast<RHI::VK_RasterizationPipeline*>(m_pipeline);
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
