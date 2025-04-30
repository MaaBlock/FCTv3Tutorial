//
// Created by Administrator on 2025/5/1.
//

#ifndef JOB_H
#define JOB_H
#include "PassResource.h"
#include "ResourceLayout.h"
#include "Vertex.h"
#include "Mesh.h"

namespace FCT {
    class VertexShader;
    class PixelShader;
    class BlendState;
    class RasterizationState;
    class DepthStencilState;

    struct TraditionPipelineState
    {
        TraditionPipelineState()
        {
            vertexShader = nullptr;
            pixelShader = nullptr;
            blendState = nullptr;
            rasterizationState = nullptr;
            depthStencilState = nullptr;
        }
        PixelLayout pixelLayout;
        VertexLayout vertexLayout;
        ResourceLayout resourceLayout;
        VertexShader* vertexShader;
        PixelShader* pixelShader;
        BlendState* blendState;
        RasterizationState* rasterizationState;
        DepthStencilState* depthStencilState;
    };

    namespace RHI
    {
        class CommandBuffer;
    }

    enum class JobType
    {
        TraditionRender,
        Submit,
    };
    struct Job : RefCount
    {
        virtual JobType getType() const = 0;
        virtual void submit(RHI::CommandBuffer* cmdBuf) = 0;
    };

    struct TraditionRenderJob : Job
    {
        JobType getType() const override
        {
            return JobType::TraditionRender;
        }
        PassResource* resource;
        TraditionPipelineState* state;
        std::vector<Mesh<uint16_t>*> meshes16;
        std::vector<Mesh<uint32_t>*> meshes32;
        bool needsUpdate;
        bool isCreated;
        TraditionRenderJob()
          : resource(nullptr)
          , state(nullptr)
          , needsUpdate(false)
          , isCreated(false)
        {
        }
        TraditionRenderJob& addMesh(Mesh<uint16_t>* mesh)
        {
            meshes16.push_back(mesh);
            needsUpdate = true;
            return *this;
        }
        TraditionRenderJob& addMesh(Mesh<uint32_t>* mesh)
        {
            meshes32.push_back(mesh);
            needsUpdate = true;
            return *this;
        }
        TraditionRenderJob& setPassResource(PassResource* resource)
        {
            this->resource = resource;
            return *this;
        }
        TraditionRenderJob& setPipelineState(TraditionPipelineState* state)
        {
            this->state = state;
            return *this;
        }
        virtual void submit(RHI::CommandBuffer* cmdBuf)
        {
            for (auto& mesh : meshes16)
            {
                mesh->bind(cmdBuf);
                mesh->draw(cmdBuf);
            }
            for (auto& mesh : meshes32)
            {
                mesh->bind(cmdBuf);
                mesh->draw(cmdBuf);
            }
        }
    };

    struct SubmitJob : Job
    {
        JobType getType() const override
        {
            return JobType::Submit;
        }
        virtual void submit(RHI::CommandBuffer* cmdBuf)
        {

        }
    };
}
#endif //JOB_H
