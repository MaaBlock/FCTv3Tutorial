//
// Created by Administrator on 2025/3/24.
//
#include "../MutilThreadBase/RefCount.h"
#include "./IPipelineResource.h"
#include "./Pipeline.h"
#ifndef RASTERIZATION_PIPELINE_H
#define RASTERIZATION_PIPELINE_H
namespace FCT
{
    namespace RHI
    {
        class RasterizationPipeline : public Pipeline
        {
        public:
            RasterizationPipeline();
            virtual void addResources(IPipelineResource* resource) = 0;
            virtual void create() = 0;
        protected:
        };
    }
}
#endif //RASTERIZATION_PIPELINE_H
