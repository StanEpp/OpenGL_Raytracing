#include "SceneReader.h"

int SceneReader::callback(p_ply_argument argument) {
	void* pData;

	//Get the function wrapper from the void pointer pData and convert its signature to the usual 
	//callback function.
	ply_get_argument_user_data(argument, &pData, NULL);
	auto f = *static_cast<std::function<int(p_ply_argument)>* >(pData);

	//Call the specific callback function
	f(argument);
	return 1;
}

void SceneReader::readScene(const char* fileName, Scene& scene){
	using namespace std::placeholders;
	
	p_ply ply = ply_open(fileName, NULL, 0, NULL);
	if(!ply) return;
	if(!ply_read_header(ply)) return;

	//Create function wrapper for each callback
	std::function<int(p_ply_argument)> v_cb = std::bind(&SceneReader::vertex_cb, this, &scene, _1);
	decltype(v_cb) t_cb = std::bind(&SceneReader::triangle_cb, this, &scene, _1);
	decltype(v_cb) m_cb = std::bind(&SceneReader::material_cb, this, &scene, _1);
	decltype(v_cb) s_cb = std::bind(&SceneReader::sphere_cb, this, &scene, _1);
	decltype(v_cb) p_cb = std::bind(&SceneReader::plane_cb, this, &scene, _1);
	decltype(v_cb) pL_cb = std::bind(&SceneReader::pointLight_cb, this, &scene, _1);
	decltype(v_cb) dL_cb = std::bind(&SceneReader::dirLight_cb, this, &scene, _1);


	//Register the callback functions. Only the function "callback" can be used as a callback function thus it is static.
	//The void* "pData" pointer is used to give the concrete function wrapper to the callback function "callback".
	//The different numbers for iData are used in the callback functions to specify which property is read at the moment.
	_nmaterial = ply_set_read_cb(ply, "material", "diffuse", callback, &m_cb, 0);
				ply_set_read_cb(ply, "material", "specular", callback, &m_cb, 1);
				ply_set_read_cb(ply, "material", "emission", callback, &m_cb, 2);
				ply_set_read_cb(ply, "material", "shininess", callback, &m_cb, 3);
	
	_nvertices = ply_set_read_cb(ply, "vertex", "x", callback, &v_cb, 0);
				ply_set_read_cb(ply, "vertex", "y", callback, &v_cb, 1);
				ply_set_read_cb(ply, "vertex", "z", callback, &v_cb, 2);

	_ntriangles = ply_set_read_cb(ply, "face", "vertex_indices",callback, &t_cb, 0);
				ply_set_read_cb(ply, "face", "material_index",callback, &t_cb, 1);

	_nplanes = ply_set_read_cb(ply, "plane", "xyz", callback, &p_cb, 0);
			ply_set_read_cb(ply, "plane", "nxyz", callback, &p_cb, 1);
			ply_set_read_cb(ply, "plane", "material_index", callback, &p_cb, 2);
			
	_nspheres = ply_set_read_cb(ply, "sphere", "xyz", callback, &s_cb, 0);
				ply_set_read_cb(ply, "sphere", "r", callback, &s_cb, 1);
				ply_set_read_cb(ply, "sphere", "material_index", callback, &s_cb, 2);

	_npointLights = ply_set_read_cb(ply, "pointLight", "xyz", callback, &pL_cb, 0);
					ply_set_read_cb(ply, "pointLight", "color", callback, &pL_cb, 1);
					ply_set_read_cb(ply, "pointLight", "attenuation", callback, &pL_cb, 2);

	_ndirLights = ply_set_read_cb(ply, "directionalLight", "xyz", callback, &dL_cb, 0);
					ply_set_read_cb(ply, "directionalLight", "color", callback, &dL_cb, 1);
					ply_set_read_cb(ply, "directionalLight", "attenuation", callback, &dL_cb, 2);

	if (!ply_read(ply)) throw std::runtime_error("Error occured while reading the scene");

	ply_close(ply);

}

int SceneReader::vertex_cb(Scene* scene, p_ply_argument argument) {
	long iData = 0;
	ply_get_argument_user_data(argument, NULL, &iData);
	float value = static_cast<float>(ply_get_argument_value(argument));

	switch(iData){
		case 0:{
			scene->getVertices()->push_back(glm::vec3(value, 0, 0));
		}break;
			
		case 1:{
			scene->getVertices()->back().y = value;
		}break;

		case 2:{
			scene->getVertices()->back().z = value;
		}break;
	}

    return 1;
}

int SceneReader::triangle_cb(Scene* scene, p_ply_argument argument) {
	long iData = 0;
	ply_get_argument_user_data(argument, NULL, &iData);
	float value = static_cast<float>(ply_get_argument_value(argument));

	switch(iData){
		case 0:{
			long value_index;
			ply_get_argument_property(argument, NULL, NULL, &value_index);
			
			switch(value_index){
				case 0:{
					Triangle t;
					t.A = glm::vec4(scene->getVertices()->at(value), 1);
					t.B = t.C = glm::vec4(0, 0, 0, 1);

					scene->getTriangles()->push_back(std::pair<Triangle, int>(t, -1));
				}break;

				case 1:{
					scene->getTriangles()->back().first.B = glm::vec4(scene->getVertices()->at(value), 1);
				}break;

				case 2:{
					scene->getTriangles()->back().first.C = glm::vec4(scene->getVertices()->at(value), 1);
				}
			}
		}break;
		case 1:{
			scene->getTriangles()->back().second = value;
		}break;
	}

    return 1;
}

int SceneReader::material_cb(Scene* scene, p_ply_argument argument) {
	long iData = 0;
	ply_get_argument_user_data(argument, NULL, &iData);
	float value = static_cast<float>(ply_get_argument_value(argument));

	switch(iData){
		case 0:{
			long value_index;
			ply_get_argument_property(argument, NULL, NULL, &value_index);
			
			switch(value_index){
				case 0:{
					Material m;
					m.diffuse = glm::vec4(value, 0, 0, 0);

					scene->getMaterials()->push_back(m);
				}break;

				case 1:{
					scene->getMaterials()->back().diffuse.y = value;
				}break;

				case 2:{
					scene->getMaterials()->back().diffuse.z = value;
				}
			}
		}break;
		case 1:{
			long value_index;
			ply_get_argument_property(argument, NULL, NULL, &value_index);
			
			switch(value_index){
				case 0:{
					scene->getMaterials()->back().specularity.x = value;
				}break;

				case 1:{
					scene->getMaterials()->back().specularity.y = value;
				}break;

				case 2:{
					scene->getMaterials()->back().specularity.z = value;
				}
			}
		}break;

		case 2:{
			long value_index;
			ply_get_argument_property(argument, NULL, NULL, &value_index);
			
			switch(value_index){
				case 0:{
					scene->getMaterials()->back().emission.x = value;
				}break;

				case 1:{
					scene->getMaterials()->back().emission.y = value;
				}break;

				case 2:{
					scene->getMaterials()->back().emission.z = value;
				}
			}
		}break;

		case 3:{
			scene->getMaterials()->back().shininess = value;
		}break;
	}

    return 1;
}

int SceneReader::plane_cb(Scene* scene, p_ply_argument argument) {
	long iData = 0;
	ply_get_argument_user_data(argument, NULL, &iData);
	float value = static_cast<float>(ply_get_argument_value(argument));

	switch(iData){
		case 0:{
			long value_index;
			ply_get_argument_property(argument, NULL, NULL, &value_index);
			
			switch(value_index){
				case 0:{
					Plane p;
					p.pos = glm::vec4(value, 0, 0, 1);
					p.normal =glm::vec4(0);

					scene->getPlanes()->push_back(std::pair<Plane, int>(p, -1));
				}break;

				case 1:{
					scene->getPlanes()->back().first.pos.y = value;
				}break;

				case 2:{
					scene->getPlanes()->back().first.pos.z = value;
				}
			}
		}break;
		case 1:{
			long value_index;
			ply_get_argument_property(argument, NULL, NULL, &value_index);
			
			switch(value_index){
				case 0:{
					scene->getPlanes()->back().first.normal.x = value;
				}break;

				case 1:{
					scene->getPlanes()->back().first.normal.y = value;
				}break;

				case 2:{
					scene->getPlanes()->back().first.normal.z = value;
				}
			}
		}break;

		case 2:{
			scene->getPlanes()->back().second = value;
		}break;
	}

    return 1;
}

int SceneReader::sphere_cb(Scene* scene, p_ply_argument argument) {
	long iData = 0;
	ply_get_argument_user_data(argument, NULL, &iData);
	float value = static_cast<float>(ply_get_argument_value(argument));

	switch(iData){
		case 0:{
			long value_index;
			ply_get_argument_property(argument, NULL, NULL, &value_index);
			
			switch(value_index){
				case 0:{
					Sphere s;
					s.center = glm::vec3(value, 0, 0);
					s.radius = 1;

					scene->getSpheres()->push_back(std::pair<Sphere, int>(s, -1));
				}break;

				case 1:{
					scene->getSpheres()->back().first.center.y = value;
				}break;

				case 2:{
					scene->getSpheres()->back().first.center.z = value;
				}
			}
		}break;

		case 1:{
			scene->getSpheres()->back().first.radius = value;
		}break;

		case 2:{
			scene->getSpheres()->back().second = value;
		}break;
	}

    return 1;
}

int SceneReader::pointLight_cb(Scene* scene, p_ply_argument argument) {
	long iData = 0;
	ply_get_argument_user_data(argument, NULL, &iData);
	float value = static_cast<float>(ply_get_argument_value(argument));

	switch(iData){
		case 0:{
			long value_index;
			ply_get_argument_property(argument, NULL, NULL, &value_index);
			
			switch(value_index){
				case 0:{
					PointLight p;
					p.position = glm::vec4(value, 0, 0, 1);
					p.color = glm::vec4(1, 1, 1, 0);
					p.attenuation = glm::vec4(0, 0, 1, 1);

					scene->getPointLights()->push_back(p);
				}break;

				case 1:{
					scene->getPointLights()->back().position.y = value;
				}break;

				case 2:{
					scene->getPointLights()->back().position.z = value;
				}
			}
		}break;

		case 1:{
			long value_index;
			ply_get_argument_property(argument, NULL, NULL, &value_index);
			
			switch(value_index){
				case 0:{
					scene->getPointLights()->back().color.x = value;
				}break;

				case 1:{
					scene->getPointLights()->back().color.y = value;
				}break;

				case 2:{
					scene->getPointLights()->back().color.z = value;
				}
			}
		}break;

		case 2:{
			long value_index;
			ply_get_argument_property(argument, NULL, NULL, &value_index);
			
			switch(value_index){
				case 0:{
					scene->getPointLights()->back().attenuation.x = value;
				}break;

				case 1:{
					scene->getPointLights()->back().attenuation.y = value;
				}break;

				case 2:{
					scene->getPointLights()->back().attenuation.z = value;
				}
			}
		}break;
	}

    return 1;
}

int SceneReader::dirLight_cb(Scene* scene, p_ply_argument argument) {
	long iData = 0;
	ply_get_argument_user_data(argument, NULL, &iData);
	float value = static_cast<float>(ply_get_argument_value(argument));

	switch(iData){
		case 0:{
			long value_index;
			ply_get_argument_property(argument, NULL, NULL, &value_index);
			
			switch(value_index){
				case 0:{
					DirectionalLight p;
					p.position = glm::vec4(value, 0, 0, 1);
					p.color = glm::vec4(1, 1, 1, 0);
					p.attenuation = glm::vec4(0, 1, 0, 2);

					scene->getDirectionalLights()->push_back(p);
				}break;

				case 1:{
					scene->getDirectionalLights()->back().position.y = value;
				}break;

				case 2:{
					scene->getDirectionalLights()->back().position.z = value;
				}
			}
		}break;

		case 1:{
			long value_index;
			ply_get_argument_property(argument, NULL, NULL, &value_index);
			
			switch(value_index){
				case 0:{
					scene->getDirectionalLights()->back().color.x = value;
				}break;

				case 1:{
					scene->getDirectionalLights()->back().color.y = value;
				}break;

				case 2:{
					scene->getDirectionalLights()->back().color.z = value;
				}
			}
		}break;

		case 2:{
			long value_index;
			ply_get_argument_property(argument, NULL, NULL, &value_index);
			
			switch(value_index){
				case 0:{
					scene->getDirectionalLights()->back().attenuation.x = value;
				}break;

				case 1:{
					scene->getDirectionalLights()->back().attenuation.y = value;
				}break;

				case 2:{
					scene->getDirectionalLights()->back().attenuation.z = value;
				}
			}
		}break;
	}

    return 1;
}
