#ifndef _LIGHTS_
#define _LIGHTS_

#include <glm\glm.hpp>

//Alignment: 16 bytes, therefore two vec4 vectors
struct PointLight{
	glm::vec4	position;
	glm::vec4	color;	
	glm::vec4	attenuation;
};

struct DirectionalLight{
	glm::vec4	position;
	glm::vec4	color;	
	glm::vec4	attenuation;
};


#endif