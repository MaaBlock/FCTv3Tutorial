#include "ShaderOutput.h"
#include <sstream>

namespace FCT {

void ShaderOutput::addOutput(PipelineAttributeType type, const std::string& name = "", DataType dataType = DataType::Float,bool flat) {
	std::string ret = name;
    TranslagteAttributeType(type, dataType);
	TranslagteDefaultName(type, ret);
    m_outputs.emplace_back(type, ret, dataType,flat);
}

std::string ShaderOutput::generateStructDefinition(const std::string& structName) const {
    std::stringstream ss;
    ss << "struct " << structName << " {\n";
    ss << "    vec4 position;\n";
    for (const auto& output : m_outputs) {
        if (output.name != "position") {
            ss << "    " << GetDataTypeName(output.dataType) << " " << output.name << ";\n";
        }
    }
    ss << "};\n";
    return ss.str();
}

std::string ShaderOutput::generateOutputDeclarations() const {
    std::stringstream ss;
    for (const auto& output : m_outputs) {
        if (output.name != "position") {
            ss << "out " << GetDataTypeName(output.dataType) << " out_" << output.name << ";\n";
        }
    }
    return ss.str();
}

std::string ShaderOutput::generateOutputAssignments(const std::string& outputVarName) const {
    std::stringstream ss;
    for (const auto& output : m_outputs) {
        if (output.name != "position") {
            ss << "    out_" << output.name << " = " << outputVarName << "." << output.name << ";\n";
        }
    }
    return ss.str();
}

} // namespace FCT