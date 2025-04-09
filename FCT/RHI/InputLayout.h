#pragma once
#include "../MutilThreadBase/RefCount.h"
#include "./IPipelineResource.h"
#include "../Context/VertexFactory.h"
#include "../Context/Format.h"
#include "../Context/Vertex.h"
#include "./ShaderBinary.h"
namespace FCT {
    namespace RHI
    {
        enum class InputRate
        {
            PerVertex,
            PerInstance
        };
        class Context;
        class VertexBuffer;
        class InputLayout : public IPipelineResource {
        public:
            PipelineResourceType getType() const
            {
                return PipelineResourceType::InputLayout;
            }
            virtual ~InputLayout() = default;
            void inputShaderCode(ShaderBinary binary)
            {
                m_shaderBinary = binary;
            }
            void add(uint32_t slot,VertexLayout vertexLayout)
            {
                m_vertexLayouts[slot] = vertexLayout;
                m_inputRates[slot] = InputRate::PerVertex;
            }
            void vertexLayout(VertexLayout vertexLayout,uint32_t slot = 0)
            {
                m_vertexLayouts[slot] = vertexLayout;
                m_inputRates[slot] = InputRate::PerVertex;
            }
            void inputRate(InputRate inputRate,uint32_t slot = 0)
            {
                m_inputRates[slot] = inputRate;
            }
            virtual void create() = 0;
        protected:
            ShaderBinary m_shaderBinary;
            std::map<uint32_t,VertexLayout> m_vertexLayouts;
            std::map<uint32_t, InputRate> m_inputRates;
            std::vector<char> m_codes;
        };
    }
} // namespace FCT
