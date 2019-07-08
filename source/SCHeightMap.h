#pragma once

#include "Core-pch.hpp"
#include "stb_image.h"

namespace sc {

	inline void Normalny(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float & rx, float & ry, float & rz)
	{
		float ax = x3 - x1,
			ay = y3 - y1,
			az = z3 - z1,
			bx = x2 - x1,
			by = y2 - y1,
			bz = z2 - z1;

		rx = ay * bz - by * az;
		ry = bx * az - ax * bz;
		rz = ax * by - bx * ay;
	}

	inline GLvoid glmNormalize(GLfloat* v)
	{
		GLfloat l;

		l = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		if (l > 0.0001f)
		{
			v[0] /= l;
			v[1] /= l;
			v[2] /= l;
		}
	}

	template <class T>
	inline T MIX(const T a, const T b, float f)
	{
		return a + (b - a) * f;
	}

	class SCHeigthMap final {

	public:

		float At(int x, int y)
		{
			if (x < 0 || y < 0 || x >= _w || y >= _h) return 0.0f;

			return _data[x + y * _w];
		}

		float *NormAt(int x, int y, int d = 0)
		{
			static float Up[] = { 0 , 1, 0 };
			if (x < 0 || y < 0 || x >= _w || y >= _h) return Up;
			
			return _normal + (x + y * _w) * 3 + d;
		}

		xVec3 GetNormal(float x, float z)
		{

			if (x < 0 || z < 0 || x > _w - 1 || z > _h - 1) return xVec3(0,1,0);

			int a, b;
			float w1, w2;

			float v[3];

			v[0] = 0.0f;
			v[1] = 1.0f;
			v[2] = 0.0f;

			a = (int)x;
			b = (int)z;
			x -= a;
			z -= b;

			{

				w1 = z * (*NormAt(a, b + 1, 0) - *NormAt(a, b, 0)) + *NormAt(a, b, 0);
				w2 = z * (*NormAt(a + 1, b + 1, 0) - *NormAt(a + 1, b, 0)) + *NormAt(a + 1, b, 0);

				v[0] = w1 + x * (w2 - w1);

				w1 = z * (*NormAt(a, b + 1, 1) - *NormAt(a, b, 1)) + *NormAt(a, b, 1);
				w2 = z * (*NormAt(a + 1, b + 1, 1) - *NormAt(a + 1, b, 1)) + *NormAt(a + 1, b, 1);

				v[1] = w1 + x * (w2 - w1);

				w1 = z * (*NormAt(a, b + 1, 2) - *NormAt(a, b, 2)) + *NormAt(a, b, 2);
				w2 = z * (*NormAt(a + 1, b + 1, 2) - *NormAt(a + 1, b, 2)) + *NormAt(a + 1, b, 2);

				v[2] = w1 + x * (w2 - w1);

			}

			glmNormalize(v);

			return xVec3(v);
		}

		float GetHeight(float x, float z)
		{

			if (x < 0 || z < 0 || x > _w - 1 || z > _h - 1) return 0.0f;

			int a = (int)x;
			int b = (int)z;

			x -= a;
			z -= b;

			if (z < 0.001f)
				return MIX(At(a, b), At(a + 1, b), x);

			if (x < 0.001f)
				return MIX(At(a, b), At(a, b + 1), z);

			if (x < z)
			{

				float w1 = z * (At(a, b + 1) - At(a, b)) + At(a, b);
				float w2 = z * (At(a + 1, b + 1) - At(a, b)) + At(a, b);

				return w1 + x / z * (w2 - w1);

			}
			else
			{

				float w1 = x * (At(a + 1, b) - At(a, b)) + At(a, b);
				float w2 = x * (At(a + 1, b + 1) - At(a, b)) + At(a, b);

				return w1 + z / x * (w2 - w1);
			}

		}

		SCHeigthMap(const std::string &filename)
		{
			int comp = 0;

			std::cout << "[INFO] Loading heightmap: " << filename << std::endl;

			unsigned char *imageData = stbi_load(filename.c_str(), &_w, &_h, &comp, 4);

			_data = new float[_w * _h];
			_normal = new float[_w * _h * 3];

			GLubyte * b = (GLubyte*)imageData;
			GLubyte * e = b + (_w * _h) * 4;
			float *d = _data;

			while (b < e)
			{

				*d++ = (*b) / 8.0f;

				b += 4;
			}

			stbi_image_free(imageData);

			float ax, ay, az, sx, sy, sz, cx, cy;
			int x, y;

			for (x = 0; x < _w; x++) {
				for (y = 0; y < _h; y++)
				{
					sx = sy = sz = 0;

					cx = x * 2;
					cy = y * 2;

					sc::Normalny(cx, At(x, y), cy, cx + 2, At(x + 1, y - 1), cy - 2, cx + 2, At(x + 1, y), cy, ax, ay, az);
					sx += ax;
					sy += ay;
					sz += az;

					sc::Normalny(cx, At(x, y), cy, cx + 2, At(x + 1, y), cy, cx, At(x, y + 1), cy + 2, ax, ay, az);
					sx += ax;
					sy += ay;
					sz += az;

					sc::Normalny(cx, At(x, y), cy, cx, At(x, y + 1), cy + 2, cx - 2, At(x - 1, y + 1), cy + 2, ax, ay, az);
					sx += ax;
					sy += ay;
					sz += az;

					sc::Normalny(cx, At(x, y), cy, cx - 2, At(x - 1, y + 1), cy + 2, cx - 2, At(x - 1, y), cy, ax, ay, az);
					sx += ax;
					sy += ay;
					sz += az;

					sc::Normalny(cx, At(x, y), cy, cx - 2, At(x - 1, y), cy, cx, At(x, y - 1), cy - 2, ax, ay, az);
					sx += ax;
					sy += ay;
					sz += az;

					float * N = NormAt(x, y);

					N[0] = sx;
					N[1] = sy;
					N[2] = sz;

					glmNormalize(N);

				}
			}
		}

	private:

		float *_data;
		float *_normal;
		int _w, _h;
	};
}
