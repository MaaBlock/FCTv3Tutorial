#pragma once
#include "../MutilThreadBase/RefCount.h"
#include "./IPipelineResource.h"
#include "../Context/VertexFactory.h"
#include "../Context/Format.h"
#include "../Context/Vertex.h"
namespace FCT {
    namespace RHI
    {
        class Context;
        class VertexBuffer;
        class InputLayout : public IPipelineResource {
        public:
            virtual ~InputLayout() = default;
            void vertexLayout(VertexLayout vertexLayout,uint32_t slot = 0)
            {
                m_vertexLayouts[slot] = vertexLayout;
            }
            virtual void create() = 0;
        protected:
            std::map<uint32_t,VertexLayout> m_vertexLayouts;
        };
    }
} // namespace FCT
