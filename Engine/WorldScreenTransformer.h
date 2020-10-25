#pragma once
#include "Graphics.h"
#include "Vec3.h"

class WorldToScreen
{
	float xFac;
	float yFac;

public:
	WorldToScreen()
		:
		xFac((float)Graphics::ScreenWidth / 2.0f),
		yFac((float)Graphics::ScreenHeight / 2.0f)
	{
	}
	
	Vec3& Transform(Vec3& WorldPoint) const
	{
		WorldPoint.x /= WorldPoint.z;
		WorldPoint.y /= WorldPoint.z;
		WorldPoint.x = (WorldPoint.x + 1.0f) * xFac;
		WorldPoint.y = (WorldPoint.y + 1.0f) * yFac;
		return WorldPoint;
	}
	Vec3 GetTransform(const Vec3& WorldPoint) const
	{
		return Transform(Vec3(WorldPoint));
	}
};