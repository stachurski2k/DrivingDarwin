#pragma once
#include<glm.hpp>
#include<vector>
using namespace std;
using namespace glm;

//One mesh can have many submeshes unconnected with each other
class CollisonSubMesh {
public:
	vector<vec2> vertices;
	bool loop=true;//whether or not connect last point with first
	CollisonSubMesh(){}
	CollisonSubMesh(bool loop) {
		this->loop = loop;
	}
	void clear() {
		vertices.clear();
	}
	void add(vec2 p) {
		vertices.push_back(p);
	}
};
/*
CollisionMesh stores verticies for colisions
*/
class CollisionMesh {
public:
	vector<CollisonSubMesh> meshes;
	void clear() {
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i].clear();
		}
		meshes.clear();
	}
	void clear(int i) {
		meshes[i].clear();
	}
	void createSubmesh(bool l) {
		meshes.push_back(CollisonSubMesh(l));
	}
	void add(int i, vec2 p) {
		meshes[i].add(p);
	}
	~CollisionMesh() {
		clear();
	}
};