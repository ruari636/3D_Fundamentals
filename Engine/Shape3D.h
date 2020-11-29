#pragma once
#include "Vec3.h"
#include <vector>

struct LineList
{
	std::vector<Vec3> vertices;
	std::vector<std::vector<int>> lines;
};

struct TriList
{
	std::vector<Vec3> vertices;
	std::vector<int> triangles;
	std::vector<bool> cullList;
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
		return { vertices,triangles,cullList };
	}
protected:
	std::vector<Vec3> vertices; //The world locations of the vertices
	std::vector<std::vector<int>> lines; //Stores lines from the first vertex
										 //in each vector to all the other vertices
	std::vector<int> triangles;
	std::vector<bool> cullList;
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

		triangles.insert(triangles.end(), { 0, 2, 1 });
		triangles.insert(triangles.end(), { 0, 1, 4 });
		triangles.insert(triangles.end(), { 0, 4, 2 });
		triangles.insert(triangles.end(), { 4, 6, 2 });
		triangles.insert(triangles.end(), { 4, 1, 5 });
		triangles.insert(triangles.end(), { 2, 3, 1 });
		triangles.insert(triangles.end(), { 2, 6, 3 });
		triangles.insert(triangles.end(), { 4, 5, 6 });
		triangles.insert(triangles.end(), { 6, 5, 7 });
		triangles.insert(triangles.end(), { 1, 3, 7 });
		triangles.insert(triangles.end(), { 1, 7, 5 });
		triangles.insert(triangles.end(), { 6, 7, 3 });
		cullList.resize(12);
	}
};