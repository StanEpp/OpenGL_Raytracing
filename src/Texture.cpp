#include "Texture.h"
#include <iostream>
Texture::Texture(int width, int height, GLuint textureFormat) : _width(width), _height(height), _textureFormat(textureFormat){
	create();
}

Texture::~Texture(){
	glDeleteTextures(1, &_textureID);
}

void Texture::create(){
	
	assert(glGetError() == GL_NO_ERROR);
	
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &_textureID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, _textureFormat, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if(glGetError() != GL_NO_ERROR){
		throw std::runtime_error("ERROR: Could not generate texture!");
	}

}

GLuint Texture::getFormat(){
	return _textureFormat; 
}

GLuint Texture::getID(){
	return _textureID;
}

int Texture::getWidth(){
	return _width;
}

int Texture::getHeight(){
	return _height;
}