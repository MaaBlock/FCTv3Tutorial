#pragma once
#include <string>
#include "../ThirdParty.h"
#include "./Vec.h"
#include "./Matrix.h"
namespace FCT
{

    enum class DataType
    {
        Float,
        Vec2,
        Vec3,
        Vec4,
        Mat2,
        Mat3,
        Mat4,
        Int,
        IVec2,
        IVec3,
        IVec4,
        UInt,
        UVec2,
        UVec3,
        UVec4,
        Bool,
        BVec2,
        BVec3,
        BVec4,
    };

    size_t GetDataTypeSize(DataType type);
    const char *GetDataTypeName(DataType type);

} // namespace FCT
