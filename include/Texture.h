#ifndef _TEXTURE_
#define _TEXTURE_

#include "ShaderManager.h"
#include <boost\shared_ptr.hpp>
#include <glm\glm.hpp>

class Texture{
private:
	int		_width, _height;

	GLuint	_textureID;
	GLuint	_textureFormat;

	void create();
public:
	Texture(int width, int height, GLuint textureFormat);
	~Texture();

	GLuint	getID();
	GLuint	getFormat();
	int		getWidth();
	int		getHeight();

};

#endif