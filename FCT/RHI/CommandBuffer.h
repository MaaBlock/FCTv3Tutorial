//
// Created by Administrator on 2025/3/31.
//
#include "../MutilThreadBase/RefCount.h"
#include "../Base/Flags.h"
#include "RasterizationPipeline.h"
#ifndef COMMANDBUFFER_H
#define COMMANDBUFFER_H


namespace FCT
{
    namespace RHI
    {
        class Semaphore;
        class Fence;
        enum class CommandBufferLevel
        {
            Primary,
            Secondary
        };
        enum class PipelineStage : size_t
        {
            colorAttachmentOutput = 0x1,
        };
        using PipelineStages = Flags<PipelineStage>;
        struct WaitSemaphoreDescription
        {
            Semaphore* semaphore;
            PipelineStages stages;
        };
        class CommandBuffer : public RefCount
        {
        public:
            CommandBuffer()
            {
                level(CommandBufferLevel::Primary);
                m_fence = nullptr;
            }
            ~CommandBuffer()
            {
                for (auto& desc : m_waitSemaphores)
                {
                    desc.semaphore->release();
                }
                for (auto& semaphore : m_signalSemaphores)
                {
                    semaphore->release();
                }
                if (m_fence)
                {
                    m_fence->release();
                }
            }
            void level(CommandBufferLevel level)
            {
                m_level = level;
            }
            virtual void reset() = 0;
            virtual void create() = 0;
            virtual void begin() = 0;
            virtual void viewport(Vec2 lt,Vec2 rb) = 0;
            virtual void scissor(Vec2 lt,Vec2 rb) = 0;
            virtual void draw(size_t vertexBegin,size_t instanceBegin,size_t vertexSize,size_t instanceSize) = 0;
            virtual void drawIndex(size_t indexBegin, size_t instanceBegin, size_t indexCount, size_t instanceCount, size_t firstVertex = 0) = 0;
            virtual void bindPipieline(RasterizationPipeline* pipeline) = 0;
            virtual void end() = 0;
            virtual void submit() = 0;
            Fence* fence() const { return m_fence; }
            void fence(Fence* fence)
            {
                FCT_SAFE_RELEASE(m_fence);
                m_fence = fence;
                FCT_SAFE_ADDREF(m_fence);
            }
            std::vector<WaitSemaphoreDescription>& waitSemaphores() { return m_waitSemaphores; }
            std::vector<Semaphore*>& signalSemaphores() { return m_signalSemaphores; }
            void clearWaitSemaphores()
            {
                for (auto& desc : m_waitSemaphores)
                {
                    desc.semaphore->release();
                }
                m_waitSemaphores.clear();
            }
            void addWaitSemaphore(Semaphore* semaphore,PipelineStages stage = PipelineStage::colorAttachmentOutput)
            {
                WaitSemaphoreDescription desc;
                desc.semaphore = semaphore;
                desc.stages = stage;
                semaphore->addRef();
                m_waitSemaphores.push_back(desc);
            }
            void addSignalSemaphore(Semaphore* semaphore)
            {
                semaphore->addRef();
                m_signalSemaphores.push_back(semaphore);
            }
        protected:
            CommandBufferLevel m_level;
            Fence* m_fence;
            std::vector<WaitSemaphoreDescription> m_waitSemaphores;
            std::vector<Semaphore*> m_signalSemaphores;
            //std::vector<Fence*> m_submitFence;
        };
    }
}
#endif //COMMANDBUFFER_H
