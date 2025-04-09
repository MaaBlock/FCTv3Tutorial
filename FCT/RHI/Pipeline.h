//
// Created by Administrator on 2025/3/24.
//
#include "../MutilThreadBase/RefCount.h"
#include "../Context/Vertex.h"
#ifndef FCT_PIPELINE_H
#define FCT_PIPELINE_H
namespace FCT
{
    enum PipelineType
    {
        Traditional,
        RayTracing,
        MeshShader,
    };
    namespace RHI
    {
        class Pipeline : public RefCount
        {
        public:
            virtual PipelineType getType() const = 0;
            void vertexLayout(VertexLayout layout,uint32_t slot = 0) { m_vertexLayouts[slot] = layout; }
            void pixelLayout(const PixelLayout& layout) { m_pixelLayout = layout; }
            virtual void create() = 0;
        protected:
            std::map<uint32_t,VertexLayout> m_vertexLayouts;
            PixelLayout m_pixelLayout;
        };
    }
}
#endif //FCT_PIPELINE_H
