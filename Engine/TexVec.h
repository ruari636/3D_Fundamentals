#pragma once
#include "Vec3.h"

class TexVec : public Vec3
{

public:
	TexVec(float u, float v, float x, float y, float z)
		:
		Vec3(x,y,z),
		uv(u, v)
	{}
	TexVec(float u, float v, Vec3 xyz)
		:
		Vec3(xyz),
		uv(u, v)
	{}
	TexVec(Vec2 uv, Vec3 xyz)
		:
		uv(uv),
		Vec3(xyz)
	{}
	TexVec(const TexVec& copy)
		:
		Vec3(copy.x, copy.y, copy.z),
		uv(copy.uv)
	{}
	TexVec& Interpolate(const TexVec& V1, float alpha)
	{
		*this = (V1 - *this) * alpha;
		return *this;
	}
	static TexVec GetInterpolated(const TexVec& V0, const TexVec& V1, float alpha)
	{
		TexVec ans = V0;
		return ans.Interpolate(V1, alpha);
	}
	TexVec operator-() const
	{
		return { -uv, Vec3(-x,-y,-z) };
	}
	TexVec operator+(const TexVec& rhs) const
	{
		return { uv + rhs.uv, Vec3(x + rhs.x, y + rhs.y, z + rhs.z) };
	}
	TexVec operator-(const TexVec& rhs) const
	{
		return (*this + (-rhs));
	}
	TexVec operator*(const float x) const
	{
		return { this->uv * x, Vec3(this->x * x, this->y * x, this->z * x) };
	}
	Vec2 uv;
};