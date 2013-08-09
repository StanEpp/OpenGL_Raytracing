#ifndef _SHADERMANAGER_
#define _SHADERMANAGER_

#ifndef _GLEW_
#define _GLEW_
	#include <GL\glew.h>
	#include <GL\glfw.h>
#endif
#include "ShaderBaseModel.h"
#include <boost\shared_ptr.hpp>
#include "textFile.h"

class ShaderManager{
private:
	boost::shared_ptr<textFile>			_txtFile;
	boost::shared_ptr<ShaderBaseModel>	_shaderData;
	static ShaderManager*				_instance;

	ShaderManager() : _txtFile(new textFile(false)), _shaderData(new ShaderBaseModel()) {}
	~ShaderManager(){}

public:
	
	
	void loadShader(const char* filepath, std::string shaderKey, GLenum type);
	void attachShader(std::string shaderKey, std::string shaderProgramKey);
	void detachShader(std::string shaderKey, std::string shaderProgramKey);

	void createProgram(std::string shaderProgramKey);
	void useProgram(std::string shaderProgramKey);
	void linkProgram(std::string shaderProgramKey);

	GLuint getShaderID(std::string shaderKey);
	GLuint getShaderProgramID(std::string shaderProgramKey);

	void deleteProgram(std::string shaderProgramKey);
	void deleteShader(std::string shaderKey);

	void resetProgram();

	void loadUintUniform(std::string shaderProgram, std::string name, GLuint value);
	void loadFloatUniform(std::string shaderProgram, std::string name, GLfloat value);
	void loadVec4Uniform(std::string shaderProgram, std::string name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void loadMatrix4Uniform(std::string shaderProgram, std::string name, const GLfloat* value);

	void getBufferVariableIndices(std::string shaderProgram, int length, const GLchar** names, GLint* indices);

	static ShaderManager* getInstance();
};


#endif