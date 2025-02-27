#pragma once
#include <cmath>

namespace FCT
{

	class Mat4
	{
	public:
		float m[16];

		Mat4()
		{
			identity();
		}
		Mat4(float m11, float m12, float m13, float m14,
			 float m21, float m22, float m23, float m24,
			 float m31, float m32, float m33, float m34,
			 float m41, float m42, float m43, float m44)
		{
			m[0] = m11;
			m[1] = m12;
			m[2] = m13;
			m[3] = m14;
			m[4] = m21;
			m[5] = m22;
			m[6] = m23;
			m[7] = m24;
			m[8] = m31;
			m[9] = m32;
			m[10] = m33;
			m[11] = m34;
			m[12] = m41;
			m[13] = m42;
			m[14] = m43;
			m[15] = m44;
		}

		void identity()
		{
			*this = {
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1};
		}
		void translate(float x, float y)
		{
			m[8] += x;
			m[9] += y;
		}
		void translate(float x, float y, float z)
		{
			Mat4 trans;
			trans.m[12] = x;
			trans.m[13] = y;
			trans.m[14] = z;
			*this = *this * trans;
		}
		void rotateX(float degrees)
		{
			float radians = degrees * 3.14159265f / 180.0f;
			float c = cosf(radians);
			float s = sinf(radians);

			Mat4 rot = {
				1, 0, 0, 0,
				0, c, -s, 0,
				0, s, c, 0,
				0, 0, 0, 1};
			*this = *this * rot;
		}
		static Mat4 CreateTranslation(float x, float y)
		{
			Mat4 ret;
			ret.m[8] += x;
			ret.m[9] += y;
			return ret;
		}
		static Mat4 CreateScale(float scaleX, float scaleY)
		{
			Mat4 ret = {
				scaleX, 0, 0, 0,
				0, scaleY, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1};
			return ret;
		}
		static Mat4 Translate(float x, float y)
		{
			Mat4 ret;
			ret.m[8] += x;
			ret.m[9] += y;
			return ret;
		}
		static Mat4 Scale(float scaleX, float scaleY)
		{
			Mat4 ret = {
				scaleX, 0, 0, 0,
				0, scaleY, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1};
			return ret;
		}
		static Mat4 Perspective(float fovy, float aspect, float zNear, float zFar)
		{
			float tanHalfFovy = tanf(fovy / 2.0f);

			Mat4 result;
			result.identity();

			result.m[0] = 1.0f / (aspect * tanHalfFovy);
			result.m[5] = 1.0f / tanHalfFovy;
			result.m[10] = -(zFar + zNear) / (zFar - zNear);
			result.m[11] = -1.0f;
			result.m[14] = -(2.0f * zFar * zNear) / (zFar - zNear);
			result.m[15] = 0.0f;

			return result;
		}
		void rotateY(float degrees)
		{
			float radians = degrees * 3.14159265f / 180.0f;
			float c = cosf(radians);
			float s = sinf(radians);

			Mat4 rot = {
				c, 0, s, 0,
				0, 1, 0, 0,
				-s, 0, c, 0,
				0, 0, 0, 1};
			*this = *this * rot;
		}
		void rotateZ(float degrees)
		{
			float radians = degrees * 3.14159265f / 180.0f;
			float c = cosf(radians);
			float s = sinf(radians);

			Mat4 rot = {
				c, -s, 0, 0,
				s, c, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1};
			*this = *this * rot;
		}

		void scale(float x, float y, float z)
		{
			Mat4 scl = {
				x, 0, 0, 0,
				0, y, 0, 0,
				0, 0, z, 0,
				0, 0, 0, 1};
			*this = *this * scl;
		}

		Mat4 operator*(const Mat4 &rhs) const
		{
			Mat4 result;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result.m[i * 4 + j] = 0.0f;
					for (int k = 0; k < 4; ++k)
					{
						result.m[i * 4 + j] += m[i * 4 + k] * rhs.m[k * 4 + j];
					}
				}
			}
			return result;
		}
	};

} // namespace FCT