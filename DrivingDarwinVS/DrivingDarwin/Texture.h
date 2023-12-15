#pragma once
#ifndef STB_IMAGE_IMPLEMENTATION

#include<string>
#include<iostream>
#include<glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
using namespace std;
/*
Texture class is responsible for loading texture data from file ,
as well as creating gl Texture object and binding it for drawing
*/
struct Texture {
	unsigned int ID;
	int width, height;
	bool created = false;
	static Texture LoadTexture(string path) {
		unsigned int id;
		glGenTextures(1, &id);

		int width, height, rgba;//rgba - how many channels texture has
		//stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &rgba, 0);
		if (data) {
			GLenum format;
			if (rgba == 1)
				format = GL_RED;
			else if (rgba == 3)
				format = GL_RGB;
			else if (rgba == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, id);// sending data to gl Texture
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			//setting clamping parameters(CLAMP_TO_EDGE forces uvs to be 0-1)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//setting filtering method(NEAREST or LINEAR) i think NEAREST will be fine for now
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			std::cout << "Texture failed to load at path: " << path << std::endl;
		}
		stbi_image_free(data);
		return { id,width,height,true };
	}
};
#endif