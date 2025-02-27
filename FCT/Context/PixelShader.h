#pragma once
#include <string>
#include "../MutilThreadBase/RefCount.h"
#include "./ShaderOutput.h"
#include "./IPipelineResource.h"

namespace FCT {
    class PixelShader : public IPipelineResource {
    public:
        PixelShader(const ShaderOutput& vertexOutput);
        virtual ~PixelShader();
        bool compileFromSource(const std::string& source);
        std::string getCompileError() const;
        const std::string& getSource() const;
        bool isCompiled() const;
        virtual bool compile() = 0;

        PipelineResourceType getType() const override;
    protected:
        std::string m_source;
        std::string m_compileError;
        bool m_isCompiled;
        const ShaderOutput& m_vertexOutput;
        std::string generateDefaultCode() const;
        std::string combineCode(const std::string& userCode) const;
        std::string getTexCoordName() const;
        std::string getTextureIdName() const;
        std::string getColorName() const;

    };
} // namespace FCT