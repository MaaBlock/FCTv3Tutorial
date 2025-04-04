//
// Created by Administrator on 2025/3/31.
//
#include "RasterizationPipeline.h"
#include "../MutilThreadBase/RefCount.h"
#include "../Base/Flags.h"
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
            void level(CommandBufferLevel level)
            {
                m_level = level;
            }
            virtual void create() = 0;
            virtual void begin() = 0;
            virtual void viewport(Vec2 lt,Vec2 rb) = 0;
            virtual void scissor(Vec2 lt,Vec2 rb) = 0;
            virtual void draw(size_t vertexBegin,size_t instanceBegin,size_t vertexSize,size_t instanceSize) = 0;
            virtual void bindPipieline(RasterizationPipeline* pipeline) = 0;
            virtual void end() = 0;
            virtual void submit() = 0;
            Fence* fence() const { return m_fence; }
            void fence(Fence* fence) { m_fence = fence; }
            std::vector<WaitSemaphoreDescription>& waitSemaphores() { return m_waitSemaphores; }
            std::vector<Semaphore*>& signalSemaphores() { return m_signalSemaphores; }
            void addWaitSemaphore(Semaphore* semaphore,PipelineStages stage = PipelineStage::colorAttachmentOutput)
            {
                WaitSemaphoreDescription desc;
                desc.semaphore = semaphore;
                desc.stages = stage;
                m_waitSemaphores.push_back(desc);
            }
            void addSignalSemaphore(Semaphore* semaphore) { m_signalSemaphores.push_back(semaphore); }
        protected:
            CommandBufferLevel m_level;
            Fence* m_fence;
            std::vector<WaitSemaphoreDescription> m_waitSemaphores;
            std::vector<Semaphore*> m_signalSemaphores;
        };
    }
}
#endif //COMMANDBUFFER_H
