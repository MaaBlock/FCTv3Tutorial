//
// Created by Administrator on 2025/4/8.
//
#include "../ThirdParty.h"
#include "./ShaderGenerate.h"
#ifndef SHADERCOMPILER_H
#define SHADERCOMPILER_H
namespace FCT
{
  class ShaderCompiler {
  public:
    virtual ~ShaderCompiler() = default;
    virtual  std::vector<char> compile(const std::string &source, const std::string &entry_point,ShaderKind kind) = 0;
  protected:

  };
}
#endif //SHADERCOMPILER_H
