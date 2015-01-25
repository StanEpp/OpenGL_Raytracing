#ifndef _TEXTURERENDERER_
#define _TEXTURERENDERER_

#define GLM_FORCE_RADIANS

#include <memory>

#include <glm\glm.hpp>
#include <glm\ext.hpp>

#include "ShaderManager.h"
#include "Texture.h"
#include "Vertex.h"
#include "Camera.h"
#include "Ray.h"

class TextureRenderer{
private:
	int							_width, _height;

	std::string					_shaderProgName;
	GLuint						_vertexUVBufferID, _VertexArrayID;
	std::shared_ptr<Texture>	_texture;

	void initialize();
public:
	TextureRenderer(int width, int height, std::shared_ptr<Texture>& texture);
	~TextureRenderer();
		
	void draw();
};

#endif