#ifndef _CAMERA_
#define _CAMERA_

#include <glm\glm.hpp>
#include "Ray.h"
#include "Includes.h"

class Camera{
public:
	int	_width, _height;
	float _tanFovY, _tanFovX;

	glm::vec3 _pos, _dir, _xAxis, _yAxis;
	glm::mat4 _transfT, _transfR;

	Camera(int resX, int resY, float fovY, glm::vec3& pos, glm::vec3& lookAt, glm::vec3& up){
		_width = resX; _height = resY;
		_pos = pos;

		_dir = glm::normalize(lookAt-pos);
		_xAxis = glm::normalize(glm::cross(up, _dir));
		_yAxis = glm::cross(-_dir, _xAxis);

		_tanFovY = glm::tan<float>(glm::radians<float>(fovY)/2.0f);
		_tanFovX = (static_cast<float>(_width)*_tanFovY)/static_cast<float>(_height);
	}

	int getWidth(){
		return _width;
	}

	int getHeight(){
		return _height;
	}

	float getTanFovY(){
		return _tanFovY;
	}

	float getTanFovX(){
		return _tanFovX;
	}

	glm::vec3 getPos(){
		return _pos;
	}

	glm::vec3 getDir(){
		return _dir;
	}

	glm::vec3 getXAxis(){
		return _xAxis;
	}

	glm::vec3 getYAxis(){
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

#ifdef __CPU
	void initRay(int x, int y, Ray& ray){
		glm::vec3 dir;
		float a, b, halfWidth, halfHeight;
		
		halfWidth = static_cast<float>(_width)/2.0f;
		halfHeight = static_cast<float>(_height)/2.0f;

		a = _tanFovX*( (static_cast<float>(x)-halfWidth+0.5f) / halfWidth);
		b = _tanFovY*( (halfHeight - static_cast<float>(y)-0.5f) / halfHeight);

		dir = glm::normalize( a*_xAxis + b*_yAxis + _dir);
		
		ray.dir = dir;
		ray.origin = _pos;
	}

#endif
};


#endif