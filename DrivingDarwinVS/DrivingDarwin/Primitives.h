#pragma once
#include"Mesh.h"
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include"Utils.h"
using namespace glm;
/*
Simple interface for drawing primitives like cubes or lines
*/
class Primitives {
public:
	static Mesh cubeMesh;
	static bool cubeInitialized;
	static void createCubeMesh() {
		if (cubeInitialized)return;
		vector<Vertex> verts;
		vector<unsigned int> tris;
		//just a cube
		verts.push_back(Vertex(vec2(-1.0, -1.0), vec2(0, 0)));
		verts.push_back(Vertex(vec2(1.0, -1.0), vec2(0, 0)));
		verts.push_back(Vertex(vec2(1.0, 1.0), vec2(0, 0)));
		verts.push_back(Vertex(vec2(-1.0, 1.0), vec2(0, 0)));
		tris.insert(tris.end(), { 0,1,2,0,2,3 });
		Primitives::cubeMesh.setupMesh();
		Primitives::cubeMesh.updateMesh(verts, tris);
		cubeInitialized = true;
	}
	static void drawCube(vec2 pos, float size, Shader* shader, vec3 color = vec3(1, 1, 1)) {
		createCubeMesh();//check if cube mesh exists
		shader->use();
		shader->setVec3("color", color);
		mat4 model = mat4(1.0f);//scal and move mat 
		model = translate(model, vec3(pos,0));
		model = scale(model, vec3(size, size, size));
		shader->setMat4("model", model);
		cubeMesh.draw(shader, false);
	}
	//draw a line from a to b
	static void drawLine(vec2 A, vec2 B, float size, Shader* shader, vec3 color = vec3(1, 1, 1)) {
		createCubeMesh();//check if cube mesh exists
		vec2 center =0.5f* (A + B) ;//for making a line just take a cube,scale it rotate it and move it
		float mag = Utils::mag(A - B);
		float angle=Utils::angle(A-B,vec2(1,0));//angle of dot product of a and b
		if (A.y < B.y) {//angles is acos so we need to check wheterher we do not have 180 degress
			angle = -angle;
		}
		mat4 model = mat4(1.0f);
		model = translate(model, vec3(center, 0));
		model = rotate(model,angle, vec3(0, 0, 1));
		model = scale(model, vec3(mag / 2, size, 1));
		shader->use();
		shader->setVec3("color", color);
		shader->setMat4("model", model);
		cubeMesh.draw(shader, false);
	}
};
Mesh Primitives::cubeMesh(true);
bool Primitives::cubeInitialized=false;
