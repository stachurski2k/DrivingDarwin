#pragma once
#include <glad/glad.h>
#include <glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
using namespace glm;

/*
Shader class is responsible for loading fragment and vertex programs as well as compiling them.
*/
class Shader {
public:
	unsigned int ID;//shader gl id
	Shader(string vertex/*path to vertex program (relative)*/, string fragment/*path to fragment program (relative)*/) {
		const char* vertCode;
		const char* fragCode;
		string svertCode;
		string sfragCode;
		ifstream vertfs(vertex), fragfs(fragment);
		stringstream vertbuf, fragbuf;
		vertbuf << vertfs.rdbuf();
		fragbuf << fragfs.rdbuf();
		svertCode = vertbuf.str();
		sfragCode = fragbuf.str();
		vertCode = svertCode.c_str();
		fragCode = sfragCode.c_str();
		//^^^ loding programs code
		fragfs.close(); vertfs.close();
		// 
		//Now compile programs and link them to this shader
		unsigned int vertID, fragID;
		//First vertex program
		vertID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertID, 1, &vertCode, NULL);
		glCompileShader(vertID);
		checkCompileErrors(vertID, "VERTEX");

		//next fragment program
		fragID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragID, 1, &fragCode, NULL);
		glCompileShader(fragID);
		checkCompileErrors(fragID, "FRAGMENT");

		//Now create shader that will contain both vertex and fragment programs
		ID = glCreateProgram();
		glAttachShader(ID, vertID);
		glAttachShader(ID, fragID);
		glLinkProgram(ID);
		
		//programs are copied to main shader
		glDeleteShader(vertID);
		glDeleteShader(fragID);
	}
	void use() {
		glUseProgram(ID);//Next drawing calls will use this shader
	}
	//Some functions for setting values in shader, values are the same in every vertex/pixel
	//In short : get location of variable in shader and set its value to v
	void setBool(const string& nm, bool v) {
		glUniform1i(glGetUniformLocation(ID, nm.c_str()), (int)v);
	}
	void setInt32(const string& nm, int v) {
		glUniform1i(glGetUniformLocation(ID, nm.c_str()), v);
	}
	void setFloat(const string& nm, float v) {
		glUniform1f(glGetUniformLocation(ID, nm.c_str()), v);
	}
	void setVec2(const string& nm, vec2 v) {
		glUniform2f(glGetUniformLocation(ID, nm.c_str()), v[0], v[1]);
	}
	void setVec3(const string& nm, vec3 v) {
		glUniform3f(glGetUniformLocation(ID, nm.c_str()), v[0], v[1],v[2]);
	}
	void setMat4(const string& nm,  mat4 v) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, nm.c_str()), 1, GL_FALSE, &v[0][0]);
	}

	void checkCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};