#pragma once
#include <string>
#include "../MutilThreadBase/RefCount.h"
#include "./VertexFactory.h"
#include "./ShaderOutput.h"
#include "./IPipelineResource.h"

namespace FCT {
    namespace RHI
    {
        class VertexShader : public IPipelineResource
        {
        public:
            PipelineResourceType getType() const override
            {
                return PipelineResourceType::VertexShader;
            }
            virtual ~VertexShader() {}
            virtual void code(const std::vector<char>& code)
            {
                m_code = code;
            }
            virtual void code(std::vector<char>&& code)
            {
                m_code = std::move(code);
            }
            virtual void create() = 0;
        protected:
            std::vector<char> m_code;
        };
    }
#ifdef FCT_DEPRECATED
    class VertexShader : public IPipelineResource {
    public:
        VertexShader(VertexFactory* factory);
        virtual ~VertexShader();
        bool compileFromSource(const std::string& source);
        std::string getCompileError() const;
        const std::string& getSource() const;
        bool isCompiled() const;
        virtual bool compile() = 0;
        void addCustomOutput(PipelineAttributeType type, const std::string& name = "", DataType dataType = DataType::Float);
        const ShaderOutput& getOutput() const { return m_output; }
        PipelineResourceType getType() const override;

    protected:
        VertexFactory* m_factory;
        std::string m_source;
        std::string m_compileError;
        bool m_isCompiled;
        ShaderOutput m_output;

        void generateCode();
        std::string combineCode(const std::string& userCode) const;
        std::string getPositionType() const;
        bool isPositionAttribute(PipelineAttributeType type) const;
		bool hasBatchId() const;
    };
#endif
} // namespace FCT
