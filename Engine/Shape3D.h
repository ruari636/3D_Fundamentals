#pragma once
#include "Vec3.h"
#include <vector>

struct LineList
{
	std::vector<Vec3> vertices;
	std::vector<std::vector<int>> lines;
};

struct TriFromLine
{
	TriFromLine(int v0, int v1, std::vector<int>&& v2s)
		:
		thirdVerts(v2s),
		lineIndex{ v0,v1 }
	{
	}

	int lineIndex[2];
	std::vector<int> thirdVerts;
};

struct TriList
{
	std::vector<Vec3> vertices;
	std::vector<TriFromLine> triangles;
};

class Shape
{
public:
	std::vector<Vec3> GetVertices() const
	{
		return vertices;
	}
	LineList GetLines() const
	{
		return { vertices, lines };
	}
	TriList GetTriangles() const
	{
		return { vertices,triangles };
	}
protected:
	std::vector<Vec3> vertices; //The world locations of the vertices
	std::vector<std::vector<int>> lines; //Stores lines from the first vertex
										 //in each vector to all the other vertices
	std::vector<TriFromLine> triangles;  //Stores each triangles tip in relation to shared lines
};

class Cube : public Shape
{
public:
	Cube(float size)
	{
		float dist = size / 2.0f;
		vertices.emplace_back(-dist, -dist, -dist);
		vertices.emplace_back(dist, -dist, -dist);
		vertices.emplace_back(-dist, dist, -dist);
		vertices.emplace_back(dist, dist, -dist);
		vertices.emplace_back(-dist, -dist, dist);
		vertices.emplace_back(dist, -dist, dist);
		vertices.emplace_back(-dist, dist, dist);
		vertices.emplace_back(dist, dist, dist);

		lines.emplace_back(std::vector<int>{0, //bottom front left corner to all connected
			1, 2, 4});
		lines.emplace_back(std::vector<int>{7, //top back right corner to all connected
			6, 5, 3});
		lines.emplace_back(std::vector<int>{3, 1, 2}); //top front right to remaining connected
		lines.emplace_back(std::vector<int>{5, 1, 4}); //bottom back right to remaining connected
		lines.emplace_back(std::vector<int>{6, 2, 4}); //top back left to remaining connected

		triangles.emplace_back(TriFromLine{ 0,1,std::vector<int>{3,4} });
		triangles.emplace_back(TriFromLine{ 0,2,std::vector<int>{3,4} });
		triangles.emplace_back(TriFromLine{ 1,5,std::vector<int>{7,4} });
		triangles.emplace_back(TriFromLine{ 3,7,std::vector<int>{1,6} });
		triangles.emplace_back(TriFromLine{ 4,6,std::vector<int>{2,5} });
		triangles.emplace_back(TriFromLine{ 6,7,std::vector<int>{5} });
		triangles.emplace_back(TriFromLine{ 2,3,std::vector<int>{6} });
	}
};