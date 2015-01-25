#ifndef _INPUTCONTROL_
#define _INPUTCONTROL_

#define GLM_FORCE_RADIANS

#include <memory>
#include <iostream>

#include "GLFWInput.h"

#include "Camera.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>

class InputControl{
private:
	std::shared_ptr<GLFWInput>	_input;
	float	_velocityRotate, _velocityTranslate;
	float	_tx, _ty, _tz, _rx, _ry, _rz;
	int		_reflectionDepth;

	void rotate(const float, const glm::vec3);
	
public:
	InputControl(float velocityRotate, float velocityTranslate) : _input(new GLFWInput()){
		_tx = _ty = _tz = _rx = _ry = _rz = 0;
		_velocityRotate = velocityRotate;
		_velocityTranslate = velocityTranslate;
	}

	~InputControl(){}

	void processInput(Camera&, int&, GLFWwindow*);

	int getReflectionDepth();

};


#endif