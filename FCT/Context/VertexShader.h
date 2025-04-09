//
// Created by Administrator on 2025/4/6.
//
#include "../MutilThreadBase/RefCount.h"
#include "Vertex.h"
#include "Context.h"
#include "../RHI/IPipelineResource.h"
#include "../RHI/ShaderBinary.h"
#ifndef FCT_VERTEXSHADER_H
#define FCT_VERTEXSHADER_H
namespace FCT {
    /*
     *包装RHI::VertexShader
     *提供自动生成Shader
     *反射shader信息
     */
    class VertexShader : public IPipelineResource
    {
    public:
        PipelineResourceType getType() const override
        {
            return PipelineResourceType::VertexShader;
        }
        VertexShader(Context* ctx)
        {
            m_ctx = ctx;
        }
        void code(std::string source)
        {
            m_userSource = source;
        }
        void generateDefaultCode();
        void compile()
        {
            m_binaryCode.code(
                m_ctx->getCompiler()->compile(m_source,"FCTEntry",ShaderKind::VertexShader)
                );
        }
        void create()
        {
            m_vertexShader = m_ctx->newRhiVertexShader();
            if (m_userSource.empty())
            {
                generateDefaultCode();
            }
            if (m_binaryCode.code().empty())
            {
                preprocess();
                compile();
            }
            m_vertexShader->code(m_binaryCode.code());
            m_vertexShader->create();
        }
        void preprocess();
        RHI::ShaderBinary binaryCode()
        {
            return m_binaryCode;
        }
        RHI::InputLayout* createBindedInputLayout()
        {
            RHI::InputLayout* il = m_ctx->createInputLayout();
            for (auto [slot, layout] : m_vertexLayouts)
            {
                il->add(slot,layout);
            }
            il->inputShaderCode(m_binaryCode);
            il->create();
            return il;
        }
        RHI::VertexShader* vertexShader()
        {
            return m_vertexShader;
        }
        void addLayout(uint32_t slot, VertexLayout layout)
        {
            m_vertexLayouts[slot] = layout;
        }
        void pixelLayout(PixelLayout layout)
        {
            m_pixelLayout = layout;
        }
    protected:
        RHI::ShaderBinary m_binaryCode;
        Context* m_ctx;
        std::map<uint32_t,VertexLayout> m_vertexLayouts;
        PixelLayout m_pixelLayout;
        RHI::VertexShader* m_vertexShader;
        std::string m_userSource;
        std::string m_source;
    };
}
#endif //FCT_VERTEXSHADER_H
