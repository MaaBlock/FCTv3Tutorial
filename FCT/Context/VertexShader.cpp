#include "VertexShader.h"
#include <sstream>
#include "DataTypes.h"

namespace FCT {

VertexShader::VertexShader(VertexFactory* factory) : m_factory(factory), m_isCompiled(false),m_output()
{
}

VertexShader::~VertexShader() {}

bool VertexShader::compileFromSource(const std::string& userCode)
{
    bool isCusstomOutputEmpty = m_output.getOutputs().empty();
    generateCode();  

    std::string finalUserCode;
    if (userCode.empty() && isCusstomOutputEmpty) {
        std::stringstream ss;
        ss << "VertexOutput main(VertexInput vs_input) {\n";
        ss << "    VertexOutput vs_output;\n";
        for (const auto& attr : m_factory->getAttributes()) {
              ss << "    vs_output." << attr.name << " = vs_input." << attr.name << ";\n";
        }
        ss << "    return vs_output;\n";
        ss << "}\n";
        finalUserCode = ss.str();
    } else if (userCode.empty() && !isCusstomOutputEmpty) {
        m_compileError = "Custom outputs defined but no user code provided. Please provide a custom vertex shader implementation.";
        return false;
    } else {
        finalUserCode = userCode;
    }

    m_source = combineCode(finalUserCode);

    return compile();
}

bool VertexShader::isPositionAttribute(PipelineAttributeType type) const {
    return type == PipelineAttributeType::Position2f ||
           type == PipelineAttributeType::Position3f ||
           type == PipelineAttributeType::Position4f;
}

bool VertexShader::hasBatchId() const
{
	for (const auto& attr : m_factory->getAttributes()) {
		if (attr.type == PipelineAttributeType::BatchId) {
			return true;
		}
	}
	return false;
}

PipelineResourceType VertexShader::getType() const
{
	return PipelineResourceType::VertexShader;
}

void VertexShader::generateCode()
{
    for (const auto& attr : m_factory->getAttributes()) {
        m_output.addOutput(attr.type, attr.name, attr.dataType,attr.flat);
    }

    std::stringstream ss;

    ss << "#version 320 es\n";
    ss << "precision highp float;\n";
    ss << "precision highp int;\n";
    ss << "precision highp sampler2D;\n";

    //ss << "#extension GL_ARB_shading_language_420pack : enable\n";

    std::string positionType = getPositionType();
    ss << "layout(binding = 0) uniform sampler2D matrixTexture;\n";
    
    ss << "layout(std140, binding = 1) uniform ViewMatrixBuffer\n";
    ss << "{\n";
    ss << "    mat4 u_viewMatrix;\n";
    ss << "};\n\n";

    ss << "layout(std140, binding = 2) uniform ProjectionMatrixBuffer\n";
    ss << "{\n";
    ss << "    mat4 u_projectionMatrix;\n";
    ss << "};\n\n";

    ss << "mat4 getMatrixFromTexture(int index) {\n";
    ss << "    return mat4(\n";
    ss << "        texelFetch(matrixTexture, ivec2(0, index), 0),\n";
    ss << "        texelFetch(matrixTexture, ivec2(1, index), 0),\n";
    ss << "        texelFetch(matrixTexture, ivec2(2, index), 0),\n";
    ss << "        texelFetch(matrixTexture, ivec2(3, index), 0)\n";
    ss << "    );\n";
    ss << "}\n\n";

    ss << "struct VertexInput {\n";
    for (const auto& attr : m_factory->getAttributes()) {
        if (isPositionAttribute(attr.type)) {
            ss << "    " << positionType << " " << attr.name << ";\n";
        } else {
            ss << "    " << GetDataTypeName(attr.dataType) << " " << attr.name << ";\n";
        }
    }
    ss << "};\n\n";

    ss << "struct VertexOutput {\n";
    for (const auto& output : m_output.getOutputs()) {
        ss << "    " << GetDataTypeName(output.dataType) << " " << output.name << ";\n";
    }
    ss << "};\n\n";

    ss << "VertexOutput fct_user_main(VertexInput);\n\n";

    int locationCounter = 0;
    for (const auto& attr : m_factory->getAttributes()) {
        if (isPositionAttribute(attr.type)) {
            ss << "layout (location = " << locationCounter << ") " << " in " << positionType << " in_" << attr.name << ";\n";
        } else {                                               
            ss << "layout (location = " << locationCounter << ") " <<" in " << GetDataTypeName(attr.dataType) << " in_" << attr.name << ";\n";
        }
        locationCounter++;
    }
    locationCounter = 0;
    ss << "\nlayout (location = 0)out vec4 gl_Position;\n\n";
    for (const auto& output : m_output.getOutputs()) {
        ss << "layout (location = " << locationCounter + 1 << ") " << (output.flat ? "flat" : "\0") << " out " << GetDataTypeName(output.dataType) << " vs2fs_" << output.name << ";\n";
        locationCounter++;
    }
    ss << "\nvoid main() {\n";
    ss << "    VertexInput vs_input;\n";
    for (const auto& attr : m_factory->getAttributes()) {
        ss << "    vs_input." << attr.name << " = in_" << attr.name << ";\n";
    }
    ss << "    VertexOutput vs_output = fct_user_main(vs_input);\n";
    for (const auto& output : m_output.getOutputs()) {
        if (isPositionAttribute(output.type)) {
            ss << "    mat4 worldMatrix = getMatrixFromTexture(0);\n";
            if (output.type == PipelineAttributeType::Position2f) {
                ss << "    vec2 position2D = vs_output." << output.name << ";\n";
                ss << "    vec3 position3D = vec3(position2D, 1.0);\n";
                ss << "    mat3 worldMatrix3x3 = mat3(worldMatrix);\n";
                ss << "    vec3 transformedPosition = worldMatrix3x3 * position3D;\n";
                ss << "    vec4 worldPos = vec4(transformedPosition.xy, 0.0, 1.0);\n"; 
                ss << "    vec4 viewPos = u_viewMatrix * worldPos;\n";
                ss << "    gl_Position = u_projectionMatrix * viewPos;\n";

            }
            else if (output.type == PipelineAttributeType::Position3f) {
                ss << "    vec4 finalPosition = vec4(vs_output." << output.name << ", 1.0);\n";
                ss << "    vec4 worldPos = worldMatrix * finalPosition;\n";
                ss << "    vec4 viewPos = u_viewMatrix * worldPos;\n";
                ss << "    gl_Position = u_projectionMatrix * viewPos;\n";
            }
            else { 
                ss << "    worldPos = worldMatrix * vs_output." << output.name << ";\n";
                ss << "    vec4 viewPos = u_viewMatrix * worldPos;\n";
                ss << "    gl_Position = u_projectionMatrix * viewPos;\n";
            }
            break;
        }
    }
    for (const auto& output : m_output.getOutputs()) {
        ss << "    vs2fs_" << output.name << " = vs_output." << output.name << ";\n";
    }
    ss << "}\n";

    m_source = ss.str();
}


std::string VertexShader::combineCode(const std::string& userCode) const
{

    std::string modifiedUserCode = userCode;
    size_t mainPos = modifiedUserCode.find("main");
    if (mainPos != std::string::npos) {
        modifiedUserCode.replace(mainPos, 4, "fct_user_main");
    }

    return m_source + "\n" + modifiedUserCode;
}

std::string VertexShader::getCompileError() const {
    return m_compileError;
}

const std::string& VertexShader::getSource() const {
    return m_source;
}

bool VertexShader::isCompiled() const {
    return m_isCompiled;
}

void VertexShader::addCustomOutput(PipelineAttributeType type, const std::string& name, DataType dataType)
{
    m_output.addOutput(type, name, dataType);
}

std::string VertexShader::getPositionType() const {
    for (const auto& attr : m_factory->getAttributes()) {
        switch (attr.type) {
        case PipelineAttributeType::Position2f:
            return "vec2";
        case PipelineAttributeType::Position3f:
            return "vec3";
        case PipelineAttributeType::Position4f:
            return "vec4";
        default:
            continue;
        }
    }
    return "vec3"; 
}

} // namespace FCT