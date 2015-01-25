#ifndef _PRIMITIVES_
#define _PRIMITIVES_

//Alignment will be round to 16bytes
struct Sphere{
	glm::vec3	center;
	float		radius;	
};

//Alignment will be 16 bytes therefore take two vec4 vectors
struct Plane{
	glm::vec4	normal;
	glm::vec4	pos;	
};

struct Triangle{
	glm::vec4	A, B, C;
};

struct Material{
	glm::vec4 diffuse;
	glm::vec4 specularity;
	glm::vec4 emission;
	float shininess;
};

struct Primitive{
	Triangle t;
	int type;
	int material_index;
};

#endif