#ifndef _SCENEREADER_
#define _SCENEREADER_

#include <functional>
#include <iostream>

#include "Scene.h"
#include "Primitives.h"
#include <rply.h>

class SceneReader{
private:
	static int	callback(p_ply_argument);

	long		_nvertices, _ntriangles, _nspheres, _nplanes, _nmaterial, _npointLights, _ndirLights;

	int vertex_cb(Scene*, p_ply_argument);
	int	pointLight_cb(Scene*, p_ply_argument);
	int dirLight_cb(Scene*, p_ply_argument);
	int material_cb(Scene*, p_ply_argument);
	int triangle_cb(Scene*, p_ply_argument);
	int sphere_cb(Scene*, p_ply_argument);
	int plane_cb(Scene*, p_ply_argument);
public:
	SceneReader(){}
	~SceneReader(){}

	void readScene(const char* filename, Scene& scene);

};


#endif