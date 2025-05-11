//
// Created by Administrator on 2025/5/1.
//
#include "./Pass.h"
#ifndef TRADITIONRENDERPASS_H
#define TRADITIONRENDERPASS_H
namespace FCT
{

class TraditionRenderPass : public Pass
{
protected:
     struct TraditionRenderJobQueue
     {
         std::vector<TraditionRenderJob*> m_traditionRenderJobs;
         std::vector<SubmitJob*> m_submitJobs;
         void clear()
         {
             for (auto& job : m_traditionRenderJobs)
             {
                 job->release();
             }
             for (auto& job : m_submitJobs)
             {
                 job->release();
             }
             m_traditionRenderJobs.clear();
             m_submitJobs.clear();
         }
     } m_jobQueues[2];
    TraditionRenderJobQueue* m_submitQueue;
    TraditionRenderJobQueue* m_currentQueue;
    std::map<TraditionPipelineState*,RHI::RasterizationPipeline*> m_pipelineStates;
public:
    TraditionRenderPass(Context* ctx)
    {
        m_submitQueue = &m_jobQueues[0];
        m_currentQueue = &m_jobQueues[1];
        m_ctx = ctx;
    }
    void submitJob(SubmitJob* job)
    {
        job->addRef();
        m_currentQueue->m_submitJobs.push_back(job);
    }
    void submitJob(TraditionRenderJob* job)
    {
        job->addRef();
        m_currentQueue->m_traditionRenderJobs.push_back(job);
    }
    void updateResource() override
    {
        std::set<PassResource*> resources;
        for (auto& job : m_submitQueue->m_traditionRenderJobs)
        {
            resources.insert(job->resource);
        }
        for (auto& res : resources)
        {
            m_addResourceCallback(res);
        }
    }
    void submit(RHI::CommandBuffer* cmdBuf) override
    {
        m_pass->beginSubmit(cmdBuf);
        for (auto& job : m_submitQueue->m_submitJobs)
        {
            job->submit(cmdBuf);
        }
        for (auto& job : m_submitQueue->m_traditionRenderJobs)
        {
            auto pipeline = getOrCreateTraditionPipeline(job->state);
            cmdBuf->bindPipieline(pipeline);
            job->resource->bind(cmdBuf,pipeline);
            job->submit(cmdBuf);
        }
        m_pass->endSubmit();
    }
    RHI::RasterizationPipeline* getOrCreateTraditionPipeline(TraditionPipelineState* state)
    {
        auto it = m_pipelineStates.find(state);
        if (it!= m_pipelineStates.end())
            return it->second;
        auto pipeline = m_ctx->createTraditionPipeline();
        pipeline->pixelLayout(state->pixelLayout);
        pipeline->vertexLayout(state->vertexLayout);
        pipeline->resourceLayout(state->resourceLayout);
        pipeline->addResources(state->vertexShader);
        pipeline->addResources(state->pixelShader);
        pipeline->addResources(state->blendState);
        pipeline->addResources(state->rasterizationState);
        pipeline->addResources(state->depthStencilState);
        pipeline->bindPass(m_pass);
        pipeline->create();
        m_pipelineStates[state] = pipeline;
        return pipeline;
    }
    void swapJobQueue() override
    {
        std::swap(m_submitQueue, m_currentQueue);
        m_currentQueue->clear();
    }
    void submit(Job* job) override
    {
        switch (job->getType())
        {
            case JobType::TraditionRender:
                submitJob(static_cast<TraditionRenderJob*>(job));
                break;
            case JobType::Submit:
                submitJob(static_cast<SubmitJob*>(job));
                break;
            default:
                break;
        }
    }
};
}
#endif //TRADITIONRENDERPASS_H
