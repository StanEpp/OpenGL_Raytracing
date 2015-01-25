#ifndef _SCENE_
#define _SCENE_

#include <vector>
#include <map>
#include <glm\glm.hpp>
#include "Primitives.h"
#include "Lights.h"

class Scene{
private:
	//Due to compatibility reasons with other .ply files the vectors contain a pair of the specific type and an integer. 
	//The second member indicates the material index. Default value = -1. In this way the primitives don't have to store the
	//material index itself if there is no material defined at all.
	std::vector<glm::vec3>					_vertices;
	std::vector<std::pair<Triangle, int> >	_triangles;
	std::vector<std::pair<Sphere, int> >	_spheres;
	std::vector<std::pair<Plane, int> >		_planes;
	std::vector<PointLight>					_pointLights;
	std::vector<DirectionalLight>			_directionalLights;
	std::vector<Material>					_materials;

public:
	Scene() = default;
	Scene(Scene& s) = delete;
	
	~Scene(){
	}

	std::vector<glm::vec3>*	getVertices(){
		return &_vertices;
	}

	std::vector<std::pair<Triangle, int> >* getTriangles(){
		return &_triangles;
	}

	std::vector<std::pair<Sphere, int> >* getSpheres(){
		return &_spheres;
	}

	std::vector<std::pair<Plane, int> >* getPlanes(){
		return &_planes;
	}

	std::vector<PointLight>* getPointLights(){
		return &_pointLights;
	}

	std::vector<DirectionalLight>* getDirectionalLights(){
		return &_directionalLights;
	}

	std::vector<Material>*	getMaterials(){
		return &_materials;
	}

	int numberOfObjects(){
		return _triangles.size() + _spheres.size() + _planes.size();
	}

	int numberOfLights(){
		return _pointLights.size() + _directionalLights.size();
	}
	
	void clear(){
		_vertices.clear();
		_triangles.clear();
		_spheres.clear();
		_planes.clear();
		_pointLights.clear();
		_directionalLights.clear();
		_materials.clear();
	}
};



#endif