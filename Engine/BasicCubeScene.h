#pragma once
#include "Scene.h"
#include "Mat3.h"
#include "Shape3D.h"
#include "WorldScreenTransformer.h"

class BasicCubeScene : public Scene
{
	Mat3 trans = Mat3::Identity();
	Cube cube;
	WorldToScreen w2s;\
	Color colourList[8]
	{
		Colors::Blue,
		Colors::Cyan,
		Colors::Gray,
		Colors::Green,
		Colors::Magenta,
		Colors::Red,
		Colors::White,
		Colors::Yellow
	};
	bool HeldDown = false;
	bool EdgeHighlight = false;

public:
	BasicCubeScene()
		:
		cube(1.0f),
		Scene( "Basic Cube With Distinguishable Triangles" )
	{
	}
	void Update(Keyboard& kbd, Mouse& mouse, float dt) override
	{
		if (kbd.KeyIsPressed('W'))
		{
			trans = Mat3::RotateX(dt) * trans;
		}
		if (kbd.KeyIsPressed('S'))
		{
			trans = Mat3::RotateX(-dt) * trans;
		}
		if (kbd.KeyIsPressed('A'))
		{
			trans = Mat3::RotateY(dt) * trans;
		}
		if (kbd.KeyIsPressed('D'))
		{
			trans = Mat3::RotateY(-dt) * trans;
		}
		if (kbd.KeyIsPressed('Q'))
		{
			trans = Mat3::RotateZ(dt) * trans;
		}
		if (kbd.KeyIsPressed('E'))
		{
			trans = Mat3::RotateZ(-dt) * trans;
		}
		if (kbd.KeyIsPressed('H'))
		{
			if (!HeldDown)
			{
				EdgeHighlight = !EdgeHighlight;
				HeldDown = true;
			}
		}
		else { HeldDown = false; }
	}
	void Draw(Graphics& gfx) override
	{
		auto cubeDraw = cube.GetEdges();
		auto cubeFill = cube.GetTriangles();
		for (Vec3& v : cubeDraw.vertices)
		{
			v = v * trans;
			v.z += 2.0f;
		}
		for (int n = 0; n < cubeFill.triangles.size(); n += 3)
		{
			Vec3 v1 = cubeDraw.vertices[cubeFill.triangles[n + 1]] - cubeDraw.vertices[cubeFill.triangles[n]];
			Vec3 v2 = cubeDraw.vertices[cubeFill.triangles[n + 2]] - cubeDraw.vertices[cubeFill.triangles[n]];
			cubeFill.cullList[n / 3] = (Vec3::Cross(v1, v2) * cubeDraw.vertices[cubeFill.triangles[n]]) >= 0;
		}
		for (Vec3& v : cubeDraw.vertices)
		{
			w2s.Transform(v);
		}

		for (int n = 0; n < cubeFill.triangles.size(); n += 3)
		{
			if (!cubeFill.cullList[n / 3])
			{
				gfx.DrawTriangle(cubeDraw.vertices[cubeFill.triangles[n]], cubeDraw.vertices[cubeFill.triangles[n + 1]],
					cubeDraw.vertices[cubeFill.triangles[n + 2]], colourList[n % 8]);
			}
		}
		if (EdgeHighlight)
		{
			for (auto curVert = cubeDraw.edges.begin(); curVert != cubeDraw.edges.end(); curVert++)
			{
				for (auto linkedVert = curVert->begin() + 1; linkedVert != curVert->end(); linkedVert++)
				{
					gfx.DrawLine(cubeDraw.vertices[*(curVert->begin())], cubeDraw.vertices[*linkedVert],
						Colors::Red);
				}
			}
		}
	}
};