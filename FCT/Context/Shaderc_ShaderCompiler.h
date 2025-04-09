//
// Created by Administrator on 2025/4/8.
//
#include "./ShaderCompiler.h"
#ifndef SHADERC_SHADERCOMPILER_H
#define SHADERC_SHADERCOMPILER_H
namespace FCT {
    constexpr shaderc_shader_kind FCTShaderKindToShaderrcShaderKind(ShaderKind kind)
    {
        switch (kind) {
        case ShaderKind::VertexShader:
            return shaderc_vertex_shader;
        case ShaderKind::FragmentShader:
            return shaderc_fragment_shader;
        case ShaderKind::ComputeShader:
            return shaderc_compute_shader;
        case ShaderKind::GeometryShader:
            return shaderc_geometry_shader;
        case ShaderKind::TessControlShader:
            return shaderc_tess_control_shader;
        case ShaderKind::MeshShader:
            return shaderc_mesh_shader;
        }
        return shaderc_vertex_shader;
    }
    class Shaderc_ShaderCompiler : public ShaderCompiler
    {
    public:
        Shaderc_ShaderCompiler();
        std::vector<char> compile(const std::string &source, const std::string &entry_point,ShaderKind kind) override;
    protected:
        shaderc::Compiler m_compiler;
        shaderc::CompileOptions m_options;
    };
}
#endif //SHADERC_SHADERCOMPILER_H
