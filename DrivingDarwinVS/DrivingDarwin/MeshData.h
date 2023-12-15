#pragma once
#include<glm.hpp>
using namespace glm;

//this data structure is used to store both position and uv in one array in mesh class
struct Vertex {
	vec3 position;
	vec2 uv;
	Vertex(vec3 p, vec2 uv) {
		this->position = p;
		this->uv = uv;
	}
	Vertex(vec2 p, vec2 uv) {
		this->position =vec3( p.x,p.y,0);
		this->uv = uv;
	}
	~Vertex(){}
};