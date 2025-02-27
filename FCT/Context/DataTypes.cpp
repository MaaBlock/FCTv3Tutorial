#include "DataTypes.h"
#include <stdexcept>

namespace FCT {

size_t GetDataTypeSize(DataType type) {
    switch (type) {
        case DataType::Float: return sizeof(float);
        case DataType::Vec2: return 2 * sizeof(float);
        case DataType::Vec3: return 3 * sizeof(float);
        case DataType::Vec4: return 4 * sizeof(float);
        case DataType::Mat2: return 4 * sizeof(float);
        case DataType::Mat3: return 9 * sizeof(float);
        case DataType::Mat4: return 16 * sizeof(float);
        case DataType::Int: return sizeof(int);
        case DataType::IVec2: return 2 * sizeof(int);
        case DataType::IVec3: return 3 * sizeof(int);
        case DataType::IVec4: return 4 * sizeof(int);
        case DataType::UInt: return sizeof(unsigned int);
        case DataType::UVec2: return 2 * sizeof(unsigned int);
        case DataType::UVec3: return 3 * sizeof(unsigned int);
        case DataType::UVec4: return 4 * sizeof(unsigned int);
        case DataType::Bool: return sizeof(bool);
        case DataType::BVec2: return 2 * sizeof(bool);
        case DataType::BVec3: return 3 * sizeof(bool);
        case DataType::BVec4: return 4 * sizeof(bool);
        default: throw std::runtime_error("Unknown data type");
    }
}

const char* GetDataTypeName(DataType type) {
    switch (type) {
        case DataType::Float: return "float";
        case DataType::Vec2: return "vec2";
        case DataType::Vec3: return "vec3";
        case DataType::Vec4: return "vec4";
        case DataType::Mat2: return "mat2";
        case DataType::Mat3: return "mat3";
        case DataType::Mat4: return "mat4";
        case DataType::Int: return "int";
        case DataType::IVec2: return "ivec2";
        case DataType::IVec3: return "ivec3";
        case DataType::IVec4: return "ivec4";
        case DataType::UInt: return "uint";
        case DataType::UVec2: return "uvec2";
        case DataType::UVec3: return "uvec3";
        case DataType::UVec4: return "uvec4";
        case DataType::Bool: return "bool";
        case DataType::BVec2: return "bvec2";
        case DataType::BVec3: return "bvec3";
        case DataType::BVec4: return "bvec4";
        default: throw std::runtime_error("Unknown data type");
    }
}

} // namespace FCT