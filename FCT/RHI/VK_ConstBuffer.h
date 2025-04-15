//
// Created by Administrator on 2025/4/14.
//
#include "./ConstBuffer.h"
#ifndef VK_CONSTBUFFER_H
#define VK_CONSTBUFFER_H
namespace FCT {
    namespace RHI
    {
        class VK_ConstBuffer : public ConstBuffer{
        public:
            VK_ConstBuffer(VK_Context* ctx);
            void mapData() override;
            void checkAndUpdateCurrentFrame();
            const vk::DescriptorBufferInfo& currentBufferInfo() {
                checkAndUpdateCurrentFrame();
                return m_bufferInfos[m_ctx->currentFrameIndex()];
            }
            const vk::DescriptorBufferInfo& currentBufferInfoWithoutUpdata() const
            {
                return m_bufferInfos[m_ctx->currentFrameIndex()];
            }
            void create() override;
        protected:
            VK_Context* m_ctx;
            std::vector<vk::Buffer> m_buffers;
            std::vector<vk::DeviceMemory> m_memories;
            std::vector<void*> m_mappedData;
            std::vector<vk::DescriptorBufferInfo> m_bufferInfos;
            std::vector<bool> m_frameNeedsUpdate;
        };
    }
}


#endif //VK_CONSTBUFFER_H
