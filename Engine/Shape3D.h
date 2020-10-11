#pragma once
#include "Vec3.h"
#include <vector>

class Shape
{
public:
	std::vector<Vec3> GetVertices() const
	{
		return vertices;
	}
	std::vector<std::vector<Vec3*>> GetLines() const
	{
		return lines;
	}

protected:
	std::vector<Vec3> vertices; //The world locations of the vertices
	std::vector<std::vector<Vec3*>> lines; //Stores lines from the first vertex
										   //in each vector to all the other vertices
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

		lines.emplace_back(std::vector<Vec3*>{&vertices[0], //bottom front left corner to all connected
			&vertices[1], &vertices[2], &vertices[4]});
		lines.emplace_back(std::vector<Vec3*>{&vertices[7], //top back right corner to all connected
			&vertices[6], &vertices[5], &vertices[3]});
		lines.emplace_back(std::vector<Vec3*>{&vertices[3], & vertices[1], & vertices[2]}); //top front right to remaining connected
		lines.emplace_back(std::vector<Vec3*>{&vertices[5], & vertices[1], & vertices[4]}); //bottom back right to remaining connected
		lines.emplace_back(std::vector<Vec3*>{&vertices[6], & vertices[2], & vertices[4]}); //top back left to remaining connected
	}
};