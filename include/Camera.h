#ifndef _CAMERA_
#define _CAMERA_

#include <glm\glm.hpp>
#include "Ray.h"

class Camera{
public:
	int		_width, _height;
	float	_tanFovY, _tanFovX;

	glm::vec3	_pos, _dir, _xAxis, _yAxis;
	glm::mat4	_transfT, _transfR;

	Camera(int resX, int resY, float fovY, glm::vec3 pos, glm::vec3 lookAt, glm::vec3 up){
		_width = resX; _height = resY;
		_pos = pos;

		_dir = glm::normalize(lookAt-pos);
		_xAxis = glm::normalize(glm::cross(up, _dir));
		_yAxis = glm::cross(-_dir, _xAxis);

		_tanFovY = glm::tan<float>(glm::radians<float>(fovY)/2.0f);
		_tanFovX = (static_cast<float>(_width)*_tanFovY)/static_cast<float>(_height);
	}

	int getWidth() const{
		return _width;
	}

	int getHeight() const{
		return _height;
	}

	float getTanFovY() const{
		return _tanFovY;
	}

	float getTanFovX() const{
		return _tanFovX;
	}

	glm::vec3 getPos() const{
		return _pos;
	}

	glm::vec3 getDir() const{
		return _dir;
	}

	glm::vec3 getXAxis() const{
		return _xAxis;
	}

	glm::vec3 getYAxis() const{
		return _yAxis;
	}

	void setTransformTranslate(glm::mat4 transf){
		_transfT = transf;
	}

	void setTransformRotate(glm::mat4 transf){
		_transfR = transf;
	}

	void update(){
		glm::vec4 temp = _transfT*glm::vec4(_pos, 1);
		_pos = glm::vec3(temp.x, temp.y, temp.z);

		temp = _transfR*glm::vec4(_dir, 0);
		_dir = glm::vec3(temp.x, temp.y, temp.z);

		temp = _transfR*glm::vec4(_xAxis, 0);
		_xAxis = glm::vec3(temp.x, temp.y, temp.z);

		temp = _transfR*glm::vec4(_yAxis, 0);
		_yAxis = glm::vec3(temp.x, temp.y, temp.z);
	}

};


#endif