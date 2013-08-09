#ifndef _TEXTURERENDERER_
#define _TEXTURERENDERER_

#include "ShaderManager.h"
#include "Texture.h"
#include "Vertex.h"
#include <boost\shared_ptr.hpp>
#include <glm\glm.hpp>
#include "Camera.h"
#include "Ray.h"

class TextureRenderer{
private:
	int									_width, _height;

	std::string							_shaderProgName;
	GLuint								_vertexUVBufferID;
	boost::shared_ptr<Texture>			_texture;

	void initialize();
public:
	TextureRenderer(int width, int height, boost::shared_ptr<Texture>& texture);
	~TextureRenderer();
		
	void draw();

};

#endif