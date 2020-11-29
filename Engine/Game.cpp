/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Game.cpp																			  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"

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

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	cube(1.0f)
{
}

void Game::Go()
{
	gfx.BeginFrame();
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	if (wnd.kbd.KeyIsPressed('W'))
	{
		trans = Mat3::RotateX(0.05f) * trans;
	}
	if (wnd.kbd.KeyIsPressed('S'))
	{
		trans = Mat3::RotateX(-0.05f) * trans;
	}
	if (wnd.kbd.KeyIsPressed('A'))
	{
		trans = Mat3::RotateY(0.05f) * trans;
	}
	if (wnd.kbd.KeyIsPressed('D'))
	{
		trans = Mat3::RotateY(-0.05f) * trans;
	}
	if (wnd.kbd.KeyIsPressed('Q'))
	{
		trans = Mat3::RotateZ(0.05f) * trans;
	}
	if (wnd.kbd.KeyIsPressed('E'))
	{
		trans = Mat3::RotateZ(-0.05f) * trans;
	}
}

void Game::ComposeFrame()
{
	auto cubeDraw = cube.GetLines();
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
	{ w2s.Transform(v);	}

	for (int n = 0; n < cubeFill.triangles.size(); n += 3)
	{
		if (!cubeFill.cullList[n / 3])
		{
			gfx.DrawTriangle(cubeDraw.vertices[cubeFill.triangles[n]], cubeDraw.vertices[cubeFill.triangles[n + 1]],
				cubeDraw.vertices[cubeFill.triangles[n + 2]], colourList[n % 8]);
		}
	}

	/*for (auto curVert = cubeDraw.lines.begin(); curVert != cubeDraw.lines.end(); curVert++)
	{
		for (auto linkedVert = curVert->begin() + 1; linkedVert != curVert->end(); linkedVert++)
		{
			gfx.DrawLine(cubeDraw.vertices[*(curVert->begin())], cubeDraw.vertices[*linkedVert],
				Colors::Red);
		}
	}*/
}