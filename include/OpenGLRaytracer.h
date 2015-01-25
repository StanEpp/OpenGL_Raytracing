#ifndef _OPENGLRAYTRACER_
#define _OPENGLRAYTRACER_

#define WORK_GROUP_SIZE 16

#include <memory>

#include "ShaderManager.h"
#include "Texture.h"
#include "Camera.h"
#include "Lights.h"
#include "Primitives.h"

class OpenGLRaytracer{
private:

	GLuint*						_storageBufferIDs;
	std::string					_shaderProgName;
	std::shared_ptr<Texture>	_texture;

	void createComputeshader(Camera&,int);

public:
	OpenGLRaytracer(std::shared_ptr<Texture>&, Camera&, int reflectionDepth);
	~OpenGLRaytracer();

	void 			renderScene(Camera& , int , int, int);
	std::string		getCompShaderProgName();
	GLuint* 		getStorageBufferIDs();
};

#endif