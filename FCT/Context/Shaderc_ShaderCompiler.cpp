//
// Created by Administrator on 2025/4/8.
//
#include "../FCTAPI.h"
namespace FCT {
    Shaderc_ShaderCompiler::Shaderc_ShaderCompiler()
    {
        m_options.SetSourceLanguage(shaderc_source_language_hlsl);
        m_options.SetTargetSpirv(shaderc_spirv_version_1_5);
        m_options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
    }

    std::vector<char> Shaderc_ShaderCompiler::compile(const std::string& source, const std::string& entry_point, ShaderKind kind)
    {
        fout << "编译着色器" << std::endl;
        fout << source << std::endl;
        auto result = m_compiler.CompileGlslToSpv(source.c_str(), source.size(), FCTShaderKindToShaderrcShaderKind(kind), "FCTShader"
            , entry_point.c_str(), m_options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            ferr << "Shader compilation failed: " << result.GetErrorMessage() << std::endl;
            return {};
        }

        const uint32_t* dataStart = result.cbegin();
        const char* byteStart = reinterpret_cast<const char*>(dataStart);
        size_t byteSize = (result.cend() - result.cbegin()) * sizeof(uint32_t);

        return std::vector<char>(byteStart, byteStart + byteSize);
    }
}
