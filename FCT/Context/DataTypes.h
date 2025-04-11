#pragma once
#include <string>
#include "../ThirdParty.h"
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

    struct Vec2
    {
        float x, y;
        Vec2(int x, int y) {
            this->x = static_cast<float>(x);
            this->y = static_cast<float>(y);
        }
        Vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
        Vec2 &operator+=(const Vec2 &rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }
        Vec2 operator+(const Vec2 &rhs) const
        {
            return Vec2(x + rhs.x, y + rhs.y);
        }
        Vec2 operator-(const Vec2 &rhs) const
        {
            return Vec2(x - rhs.x, y - rhs.y);
        }
        Vec2 operator*(float scalar) const
        {
            return Vec2(x * scalar, y * scalar);
        }
        Vec2 operator/(float scalar) const
        {
            return Vec2(x / scalar, y / scalar);
        }
        Vec2 operator/(const Vec2 &rhs) const
        {
            return Vec2(x / rhs.x, y / rhs.y);
        }
        bool operator==(const Vec2& other) const
        {
            return (x == other.x) && (y == other.y);
        }

    };

    struct Vec3
    {
#ifdef FCT_USE_PHYSX
        operator physx::PxExtendedVec3() const {
            return physx::PxExtendedVec3(x, y, z);
        }
        operator physx::PxVec3() const {
            return physx::PxVec3(x, y, z);
        }
        Vec3(const physx::PxVec3& other) {
			x = other.x;
		    y = other.y;
		    z = other.z;
		}
#endif
        float x, y, z;
        Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

        Vec3(const Vec3 &other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
        }

        bool operator==(const Vec3 &other) const
        {
            return (x == other.x) && (y == other.y) && (z == other.z);
        }

        Vec3 &operator+=(const Vec3 &rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }
        bool operator<(const Vec3 &other) const
        {
            return (x < other.x) && (y < other.y) && (z < other.z);
        }

        float lengthSquared() const
        {
            return x * x + y * y + z * z;
        }
        Vec3 &operator-=(const Vec3 &rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }
        Vec3 operator+(const Vec3 &rhs) const
        {
            return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
        }

        Vec3 operator-(const Vec3 &rhs) const
        {
            return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
        }
        Vec3 operator*(float scalar) const
        {
            return Vec3(x * scalar, y * scalar, z * scalar);
        }
        float length() const
        {
            return std::sqrt(x * x + y * y + z * z);
        }
        Vec3 normalize() const
        {
            float length = std::sqrt(x * x + y * y + z * z);
            return Vec3(x / length, y / length, z / length);
        }
        Vec3 cross(const Vec3 &other) const
        {
            return Vec3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x);
        }
        Vec3 rotate(const Vec3 &axis, float angle) const
        {
            float cosTheta = std::cos(angle);
            float sinTheta = std::sin(angle);
            Vec3 normalizedAxis = axis.normalize();

            return Vec3(
                (cosTheta + (1 - cosTheta) * normalizedAxis.x * normalizedAxis.x) * x +
                    ((1 - cosTheta) * normalizedAxis.x * normalizedAxis.y - sinTheta * normalizedAxis.z) * y +
                    ((1 - cosTheta) * normalizedAxis.x * normalizedAxis.z + sinTheta * normalizedAxis.y) * z,

                ((1 - cosTheta) * normalizedAxis.x * normalizedAxis.y + sinTheta * normalizedAxis.z) * x +
                    (cosTheta + (1 - cosTheta) * normalizedAxis.y * normalizedAxis.y) * y +
                    ((1 - cosTheta) * normalizedAxis.y * normalizedAxis.z - sinTheta * normalizedAxis.x) * z,

                ((1 - cosTheta) * normalizedAxis.x * normalizedAxis.z - sinTheta * normalizedAxis.y) * x +
                    ((1 - cosTheta) * normalizedAxis.y * normalizedAxis.z + sinTheta * normalizedAxis.x) * y +
                    (cosTheta + (1 - cosTheta) * normalizedAxis.z * normalizedAxis.z) * z);
        }
    };
    inline Vec3 normalize(const Vec3 &v)
    {
        float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        return Vec3(v.x / length, v.y / length, v.z / length);
    }

    inline Vec3 cross(const Vec3 &a, const Vec3 &b)
    {
        return Vec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x);
    }

    inline float dot(const Vec3 &a, const Vec3 &b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    struct Vec4
    {
        float x, y, z, w;
        Vec4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f)
            : x(x), y(y), z(z), w(w) {}
    };

} // namespace FCT
