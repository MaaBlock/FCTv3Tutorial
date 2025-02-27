#include "../headers.h"
#include <sstream>

namespace FCT {

    PixelShader::PixelShader(const ShaderOutput& vertexOutput)
        : m_isCompiled(false), m_vertexOutput(vertexOutput)
    {
    }

    PixelShader::~PixelShader() {}

    bool PixelShader::compileFromSource(const std::string& userCode)
    {
        std::string finalUserCode = userCode.empty() ? generateDefaultCode() : userCode;
        m_source = combineCode(finalUserCode);
        return compile();
    }

    std::string PixelShader::generateDefaultCode() const
    {
        std::stringstream ss;
        ss << "PixelOutput main(PixelInput ps_input) {\n";
        ss << "    PixelOutput ps_output;\n";

        bool hasColorAttribute = false;
        for (const auto& output : m_vertexOutput.getOutputs()) {
            if (output.type == PipelineAttributeType::Color4f) {
                hasColorAttribute = true;
            }
            ss << "    ps_output." << output.name << " = ps_input." << output.name << ";\n";
        }

        if (!hasColorAttribute) {
            ss << "    ps_output.color = vec4(1.0, 0.647, 0.0, 1.0);\n";
        }

        ss << "    return ps_output;\n";
        ss << "}\n";
        return ss.str();
    }

    std::string PixelShader::combineCode(const std::string& userCode) const
    {
        std::stringstream ss;
        ss << "#version 320 es\n\n";
        ss << "precision highp float;\n";
        ss << "precision highp int;\n";
        ss << "precision highp sampler2D;\n";
        ss << "precision highp sampler2DArray;\n";

        bool hasColorAttribute = false;
        bool hasTexCoordAttribute = false;
        bool hasTextureId = false;
        for (const auto& output : m_vertexOutput.getOutputs()) {
            if (output.type == PipelineAttributeType::Color4f) {
                hasColorAttribute = true;
            }
            if (output.type == PipelineAttributeType::TexCoord2f) {
                hasTexCoordAttribute = true;
            }
            if (output.type == PipelineAttributeType::TextureId) {
                hasTextureId = true;
            }
        }
        //ss << "#extension GL_ARB_shading_language_420pack : enable\n";
        if (hasTextureId) {
            ss << "layout(binding = 3) uniform sampler2DArray mainTexture;\n\n";
        }
        else {
            ss << "layout(binding = 3) uniform sampler2D mainTexture;\n\n";
        }
        
        ss << "struct PixelInput {\n";
        for (const auto& output : m_vertexOutput.getOutputs()) {
            ss << "    " << GetDataTypeName(output.dataType) << " " << output.name << ";\n";
        }
        ss << "};\n\n";

        ss << "struct PixelOutput {\n";
        for (const auto& output : m_vertexOutput.getOutputs()) {
            ss << "    " << GetDataTypeName(output.dataType) << " " << output.name << ";\n";
        }
        if (!hasColorAttribute) {
            ss << "    vec4 color;\n";
        }
        ss << "};\n\n";

        int locationCounter = 0;
        for (const auto& output : m_vertexOutput.getOutputs()) {
            ss << "layout (location = " << locationCounter + 1 << ") " << (output.flat ? "flat" : "\0") << " in " << GetDataTypeName(output.dataType) << " vs2fs_" << output.name << ";\n";
            locationCounter++;
        }
        ss << "\nlayout (location = 0)out vec4 FragColor;\n\n";
        ss << "PixelOutput fct_user_main(PixelInput);\n\n";

        ss << "void main() {\n";
        ss << "    PixelInput ps_input;\n";
        for (const auto& output : m_vertexOutput.getOutputs()) {
            ss << "    ps_input." << output.name << " = vs2fs_" << output.name << ";\n";
        }
        if (hasTexCoordAttribute) {
            if (hasColorAttribute) {
                if (hasTextureId) {
                    ss << "ps_input." << getColorName() << " *= texture(mainTexture, vec3(ps_input." << getTexCoordName() << ",ps_input." << getTextureIdName() << ")); \n";
                }
                else {
                    ss << "ps_input." << getColorName() << " *= texture(mainTexture, ps_input." << getTexCoordName() << "); \n";
                }
			}
			else {
                if (hasTextureId) {
                    ss << "ps_input." << getColorName() << " = texture(mainTexture, vec3(ps_input." << getTexCoordName() << ",ps_input." << getTextureIdName() << ")); \n";
                }
                else {
                    ss << "ps_input." << getColorName() << " = texture(mainTexture, ps_input." << getTexCoordName() << "); \n";
                }
            }
        }
        ss << "    PixelOutput ps_output = fct_user_main(ps_input);\n";
        if (hasColorAttribute) {
            for (const auto& output : m_vertexOutput.getOutputs()) {
                if (output.type == PipelineAttributeType::Color4f) {
                    ss << "    FragColor = ps_output." << output.name << ";\n";
                    break;
                }
            }
        }
        else {
            ss << "    FragColor = ps_output.color;\n";
        }
        ss << "}\n\n";
        std::string modifiedUserCode = userCode;
        size_t mainPos = modifiedUserCode.find("main");
        if (mainPos != std::string::npos) {
            modifiedUserCode.replace(mainPos, 4, "fct_user_main");
        }

        ss << modifiedUserCode;

        return ss.str();
    }
    PipelineResourceType PixelShader::getType() const
    {
		return PipelineResourceType::PixelShader;
    }
    std::string PixelShader::getCompileError() const {
        return m_compileError;
    }

    const std::string& PixelShader::getSource() const {
        return m_source;
    }

    bool PixelShader::isCompiled() const {
        return m_isCompiled;
    }

    std::string PixelShader::getTexCoordName() const {
        for (const auto& output : m_vertexOutput.getOutputs()) {
            if (output.type == PipelineAttributeType::TexCoord2f) {
                return output.name;
            }
        }
        return "";
    }
	std::string PixelShader::getTextureIdName() const {
		for (const auto& output : m_vertexOutput.getOutputs()) {
			if (output.type == PipelineAttributeType::TextureId) {
				return output.name;
			}
		}
	}

    std::string PixelShader::getColorName() const {
        for (const auto& output : m_vertexOutput.getOutputs()) {
            if (output.type == PipelineAttributeType::Color4f) {
                return output.name;
            }
        }
        return "color";
    }

} // namespace FCT
