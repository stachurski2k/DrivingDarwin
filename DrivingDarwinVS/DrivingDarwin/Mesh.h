#pragma once
#include<vector>
#include<glad/glad.h>
#include"MeshData.h"
#include"Texture.h"
#include"Shader.h"
using namespace std;
/*Mesh class is responsible of storing mesh data(vertices, uvs and texture) 
as well as binding them to draw on the screen*/
class Mesh {
public:
	unsigned int VAO,VBO,EBO;
	/* Those are indexes of coresponding objects
	VBO(vertex buffer object) - buffer containing vertex data
	EBO(element buffer object) - buffer containing indexes of VBO (Commonly known as triangles)
	VAO(vertex array object) - wraper for VBO and EBO as well as settings for drawing them
	*/
	vector<Vertex> vertices;
	vector<unsigned int> triangles;
	Texture tex;
	Mesh(bool static){}
	Mesh() {
		setupMesh();
	}
	Mesh(const vector<Vertex>& v,const vector<unsigned int>& t) {
		setupMesh();
		updateMesh(v, t);
	}
	void updateMesh(const vector<Vertex>& v,const vector<unsigned int>& t) {
		vertices.clear();
		triangles.clear();

		vertices.insert(vertices.end(), v.begin(), v.end());
		triangles.insert(triangles.end(), t.begin(), t.end());

		updateMesh();
	}
	void updateMesh() {
		if (vertices.size() == 0 || triangles.size() == 0) { return; }
		glBindBuffer(GL_ARRAY_BUFFER, VBO);//coping data from vertices to VBO
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);//coping data from vertices to EBO 
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(int), &triangles[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void setTexture(Texture tex) {
		this->tex = tex;
	}
	void setupMesh() {
		glGenVertexArrays(1,&VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		//binding EBO and VBO to VAO, VAO can have up to 15 buffers binded and enabled
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		//following consider only GL_ARRAY_BUFFER
		glEnableVertexAttribArray(0);//enabling array with index 0 of VAO
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(Vertex), (void*)0);
		//setting first array of VAO to array of Vertex.position

		glEnableVertexAttribArray(1);//enabling array with index 1 of VAO
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*)offsetof(Vertex,uv));
		//setting second array of VAO to array of Vertex.position

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void draw(Shader* s,bool useTexture=false) {
		//draw this mesh with shader
		s->use();
		if (useTexture && tex.created) {
			glActiveTexture(GL_TEXTURE0);//There can be 15 active textures in shader, just get first and bind mesh texture to it
			glBindTexture(GL_TEXTURE_2D, tex.ID);
		}
		//Drawing now
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, triangles.size(),GL_UNSIGNED_INT,0);
		glBindVertexArray(0);
	}
	void clear() {
		vertices.clear();
		triangles.clear();

	}
	~Mesh() {
		vertices.clear();
		triangles.clear();
		//to do clear gl buffers
	}
};