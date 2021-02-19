#pragma once
#include "Vec3.h"
#include <vector>

struct LineList
{
	std::vector<Vec3> vertices;
	std::vector<std::vector<int>> edges;
};

template <typename T>
struct TriList
{
	std::vector<T> vertices;
	std::vector<int> triangles;
	std::vector<bool> cullList;
};

class Shape
{
public:
	std::vector<Vec3> GetVertices() const 
	{
		return triList.vertices;
	}
	LineList GetEdges() const
	{
		return { triList.vertices, edges };
	}
	TriList<Vec3> GetTriangles() const
	{
		return { triList.vertices,triList.triangles,triList.cullList };
	}
protected:
	std::vector<std::vector<int>> edges; //Stores edges from the first vertex
										 //in each vector to all the other vertices
	TriList<Vec3> triList;
};

class Cube : public Shape
{
public:
	Cube(float size)
	{
		float dist = size / 2.0f;
		triList.vertices.emplace_back(-dist, -dist, -dist);
		triList.vertices.emplace_back(dist, -dist, -dist);
		triList.vertices.emplace_back(-dist, dist, -dist);
		triList.vertices.emplace_back(dist, dist, -dist);
		triList.vertices.emplace_back(-dist, -dist, dist);
		triList.vertices.emplace_back(dist, -dist, dist);
		triList.vertices.emplace_back(-dist, dist, dist);
		triList.vertices.emplace_back(dist, dist, dist);

		edges.emplace_back(std::vector<int>{0, //bottom front left corner to all connected
			1, 2, 4});
		edges.emplace_back(std::vector<int>{7, //top back right corner to all connected
			6, 5, 3});
		edges.emplace_back(std::vector<int>{3, 1, 2}); //top front right to remaining connected
		edges.emplace_back(std::vector<int>{5, 1, 4}); //bottom back right to remaining connected
		edges.emplace_back(std::vector<int>{6, 2, 4}); //top back left to remaining connected

		triList.triangles.insert(triList.triangles.end(), { 0, 2, 1 });
		triList.triangles.insert(triList.triangles.end(), { 0, 1, 4 });
		triList.triangles.insert(triList.triangles.end(), { 0, 4, 2 });
		triList.triangles.insert(triList.triangles.end(), { 4, 6, 2 });
		triList.triangles.insert(triList.triangles.end(), { 4, 1, 5 });
		triList.triangles.insert(triList.triangles.end(), { 2, 3, 1 });
		triList.triangles.insert(triList.triangles.end(), { 2, 6, 3 });
		triList.triangles.insert(triList.triangles.end(), { 4, 5, 6 });
		triList.triangles.insert(triList.triangles.end(), { 6, 5, 7 });
		triList.triangles.insert(triList.triangles.end(), { 1, 3, 7 });
		triList.triangles.insert(triList.triangles.end(), { 1, 7, 5 });
		triList.triangles.insert(triList.triangles.end(), { 6, 7, 3 });
		triList.cullList.resize(12);
	}
};