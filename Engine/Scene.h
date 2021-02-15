#pragma once
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <string>

class Scene
{
	const std::string name;

public:
	Scene(const std::string& name)
		:
		name(name)
	{}
	virtual ~Scene() = default;
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) = 0;
	virtual void Draw(Graphics& gfx) = 0;
	std::string GetName() const
	{
		return name;
	}
};