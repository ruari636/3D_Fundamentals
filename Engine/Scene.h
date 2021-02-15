#pragma once
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"

template <typename T>
class Scene
{
public:
	Scene<T>()
	{
	}
	void Draw(Graphics& gfx);
	void Update(Keyboard& kbd, Mouse& mouse, float dt);
};