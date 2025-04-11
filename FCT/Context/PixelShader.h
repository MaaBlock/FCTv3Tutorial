//
// Created by Administrator on 2025/4/9.
//
#include "../MutilThreadBase/RefCount.h"
#include "Vertex.h"
#include "Context.h"
#include "../RHI/IPipelineResource.h"
#include "../RHI/ShaderBinary.h"
#ifndef FCT_PIXELSHADER_H
#define FCT_PIXELSHADER_H
namespace FCT {
    class PixelShader : public IPipelineResource {
    public:
        PipelineResourceType getType() const override
        {
            return PipelineResourceType::PixelShader;
        }
        PixelShader(Context* ctx)
        {
            m_ctx = ctx;
        }
        void code(std::string source)
        {
            m_userSource = source;
        }
        void generateDefaultCode()
        {
            m_userSource = m_ctx->getGenerator()->generateDefaultPixelMain(m_pixelLayout);
        }
        void compile()
        {
            m_binaryCode.code(
                m_ctx->getCompiler()->compile(m_source,"FCTEntry",ShaderKind::FragmentShader)
                );
        }
        void create()
        {
            m_pixelShader = m_ctx->newRhiPixelShader();
            if (m_userSource.empty())
            {
                generateDefaultCode();
            }
            if (m_binaryCode.code().empty())
            {
                preprocess();
                compile();
            }
            m_pixelShader->code(m_binaryCode.code());
            m_pixelShader->create();
        }
        void preprocess()
        {
            m_source = m_ctx->getGenerator()->generatePixelShader(m_pixelLayout,m_uniformLayouts,m_binaryCode, m_userSource);
        }
        RHI::ShaderBinary binaryCode()
        {
            return m_binaryCode;
        }
        RHI::PixelShader* pixelShader()
        {
            return m_pixelShader;
        }
        void pixelLayout(const PixelLayout& layout)
        {
            m_pixelLayout = layout;
        }
        void addUniform(UniformLayout layout)
        {
            m_uniformLayouts.push_back(layout);
        }
    protected:
        RHI::ShaderBinary m_binaryCode;
        Context* m_ctx;
        PixelLayout m_pixelLayout;
        RHI::PixelShader* m_pixelShader;
        std::string m_userSource;
        std::string m_source;
        std::vector<UniformLayout> m_uniformLayouts;
    };

}
#endif //FCT_PIXELSHADER_H
