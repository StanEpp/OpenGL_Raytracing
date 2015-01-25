#version 430
#define FAR_CLIP  1000.0f

struct Camera{
	vec4	pos, dir, yAxis, xAxis ;
	float	tanFovY, tanFovX;
};

//attenuation.w : 1 = Point light; 2 = Directional light
struct Light{
	vec4	pos_dir;
	vec4	color;
	vec4	attenuation;
};

struct Material{
	vec4 diffuse;
	vec4 specularity;
	vec4 emission;
	float shininess;	
};

struct Primitive{
	vec4 A, B, C;
};

//type: 1 = Sphere; 2 = Plane; 3 = Triangle
struct Object{
	Primitive p;
	int type;
	int material_index;
};

struct Ray{
	vec3	origin;
	vec3	dir;
};

layout(std430) buffer PrimitiveBuffer{
	Object objects[];
};

layout(std430) buffer MaterialBuffer{
	Material materials[];
};

layout(std430) buffer LightBuffer{
	Light lights[];
};

uniform Camera 		camera;
uniform uint		width;
uniform uint		height;
uniform uint		numObj;
uniform uint		numLights;
uniform uint		reflectionDepth;

Object objectss[1];
Material materialss[1];
Light lightss[2];
uint numObjj = 1;
uint numLightss = 2;

writeonly uniform image2D outputTexture;

//ray.dir has to be normalized
float hitSphere(Ray r, Primitive s){
	
	vec3 oc = r.origin - s.A.xyz;
	float s_roc = dot(r.dir, oc);
	float s_oc = dot(oc, oc);
	
	float d = s_roc*s_roc - s_oc + s.A.w*s.A.w;

	if(d < 0){
		return FAR_CLIP;
	} else if(d == 0) {
		if(-s_roc < 0){
			return FAR_CLIP;
		}

		return -s_roc;
	} else {
		float t1 = 0, t2 = 0;
		
		t1 = sqrt(d);
		t2 = -s_roc-t1;
		t1 = -s_roc+t1;
		
		//ray origin lies in the sphere
		if( (t1 < 0 && t2 > 0)  || (t1 > 0 && t2 <0)){
			return FAR_CLIP;
		}
		
		if( (t2>t1 ? t1 : t2) < 0){
			return FAR_CLIP;
		} else {
			return (t2>t1 ? t1 : t2);
		}
	}
}

float hitPlane(Ray r, Primitive p){

	float s_nr = dot(p.B.xyz, r.dir);
	
	if(s_nr <= 0.00001f && s_nr >= -0.00001f){
		return FAR_CLIP;
	} else {
		float s_nv = dot(p.A.xyz, p.B.xyz);
		float s_no = dot(p.B.xyz, r.origin);
		
		return ((s_nv-s_no)/s_nr);
	}
	
}

float hitTriangle(Ray r, Primitive t){
	vec3 AB = t.B.xyz - t.A.xyz;
	vec3 AC = t.C.xyz - t.A.xyz;

	float det = determinant( mat3(AB, AC, -1.0f*r.dir) );
	
	if(det == 0.0f){
		return FAR_CLIP;
	} else {
		vec3 oA = r.origin - t.A.xyz;
		
		mat3 Di = inverse(mat3(AB, AC, -1.0f*r.dir));
		vec3 solution = Di*oA;

		if(solution.x >= -0.0001 && solution.x <= 1.0001){
			if(solution.y >= -0.0001 && solution.y <= 1.0001){
				if(solution.x + solution.y <= 1.0001){
					return solution.z;
				}
			}
		}
		return FAR_CLIP;
	}
}

Ray initRay(uint x, uint y, Camera cam){
	Ray r;
	vec3 dir;
	float a, b, halfWidth, halfHeight;
		
	halfWidth = float(width)/2.0f;
	halfHeight = float(height)/2.0f;

	a = cam.tanFovX*( (float(x)-halfWidth+0.5f) / halfWidth);
	b = cam.tanFovY*( (halfHeight - float(y)-0.5f) / halfHeight);

	dir = normalize( a*cam.xAxis.xyz + b*cam.yAxis.xyz + cam.dir.xyz);
		
	r.dir = dir;
	r.origin = cam.pos.xyz;
	
	return r;
}

Ray getReflectionRay(Ray r, int currentObject, float t){
	vec3 hitPoint = r.origin + r.dir * t;
	vec3 N = vec3(0,0,0);
	
	switch(objectss[currentObject].type){
		case 1:{
			N = normalize(hitPoint - objectss[currentObject].p.A.xyz);
		} break;
	
		case 2:{
			N = normalize(objectss[currentObject].p.B.xyz);
		} break;
	
		case 3:{
			N = normalize(cross((objectss[currentObject].p.B - objectss[currentObject].p.A).xyz, 
								(objectss[currentObject].p.C - objectss[currentObject].p.A).xyz));					
		} break;
	}
	
	vec3 dir = normalize( r.dir - 2 * dot(r.dir, N) * N);
	Ray ray = { hitPoint+dir*0.01f, dir};
	
	return ray;
}

vec4 calculateColor(Ray r, float t, int currentObject){
	vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	if(currentObject != -1){
		vec3 hitPoint = r.origin + t*r.dir;
		vec3 N, L, H, attCoef;
		Ray shadowRay;
		bool inShadow = false;
		bool lightet = false;
		float temp = FAR_CLIP;
		int	x = -1, lightType = 1;
		
		switch(objectss[currentObject].type){
			case 1:{
				N = normalize(hitPoint - objectss[currentObject].p.A.xyz);
			} break;
		
			case 2:{
				N = normalize(objectss[currentObject].p.B.xyz);
				
				//Mirror the normal if the camera's position is at the other side of the plane. 
				//This avoids considering light sources behind the plane.
				if( dot(N, camera.pos.xyz - hitPoint) < 0){
					N = -1.0f*N;
				}
			} break;
		
			case 3:{
				N = normalize(cross((objectss[currentObject].p.B - objectss[currentObject].p.A).xyz, 
									(objectss[currentObject].p.C - objectss[currentObject].p.A).xyz));					
			} break;
		}

		hitPoint += 0.01f*N;
		
		for(int j = 0; j < numLightss; ++j){
			inShadow = false;
			
			lightType = int(lightss[j].attenuation.w);

			switch(lightType){
				//Point light
				case 1:{
					L = normalize(lightss[j].pos_dir.xyz - hitPoint);
				}break;

				//Directional light
				case 2:{
					L = normalize(lightss[j].pos_dir.xyz);
				}break;
			}

			shadowRay = Ray( hitPoint, L);
			
			for(int i = 0; i < numObjj; ++i){
				switch(objectss[i].type){
					case 1:{
						temp = hitSphere(shadowRay, objectss[i].p);
					} break;
			
					case 2:{
						temp = hitPlane(shadowRay, objectss[i].p);
					} break;
			
					case 3:{
						temp = hitTriangle(shadowRay, objectss[i].p);
					} break;
				}

				switch(lightType){
					//Point light
					case 1:{
						if( (temp < FAR_CLIP && temp >= -0.001f && temp < length(hitPoint - lightss[j].pos_dir.xyz))){
							inShadow = true;
							i = int(numObjj);
						}
					} break;

					//Directional light
					case 2:{
						if(temp < FAR_CLIP && temp >= -0.001f){
							inShadow = true;
							i = int(numObjj);
						}
					} break;
				}

				
			}

			if(!inShadow){
		
				H = normalize(L + normalize(camera.pos.xyz-hitPoint));

				if(dot(N, L) > 0){
					attCoef = lightss[j].color.xyz / ( lightss[j].attenuation.x +
							  lightss[j].attenuation.y * length(lightss[j].pos_dir.xyz - hitPoint)  +
							  lightss[j].attenuation.z * pow( length(lightss[j].pos_dir.xyz - hitPoint)*0.1f , 2) );
						  
					x = objectss[currentObject].material_index;

					if(x != -1){
						color += vec4( attCoef * (materialss[x].diffuse.xyz * max(dot(N, L), 0) + 
									   materialss[x].specularity.xyz * pow( max( dot(-N,H), 0), materialss[x].shininess)), 
									   0.0f);
					} else {
						color += vec4( attCoef * (vec3(0.5,0.5,0.5) * max(dot(N, L), 0) + 
									   vec3(0.5,0.5,0.5) * pow( max( dot(N,H), 0), 10)), 
									   0.0f);
					}
				}

				inShadow = false;
				lightet = true;
			}
			
		}

		if(lightet){
			if(x!=-1){
				color += materialss[objectss[currentObject].material_index].emission;
			}

		}
	
	}

	return color;
}

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main(){
	uint x = gl_GlobalInvocationID.x;
	uint y = gl_GlobalInvocationID.y;
	
	objectss[0].type = 1;
	objectss[0].p.A = vec4(0, 0, 10, 4);
	objectss[0].p.B = objectss[0].p.C = vec4(0);
	objectss[0].material_index = 0;
	
	materialss[0].diffuse = vec4(1, 0.75, 1, 0);
	materialss[0].specularity = vec4(0.1, 0.1, 0.1, 0);
	materialss[0].emission = vec4(0);
	materialss[0].shininess = 1;
	
	lightss[0].pos_dir = vec4(0, 0,0, 0);
	lightss[0].color = vec4(1,1,1,0);
	lightss[0].attenuation = vec4(0, 0, 0.5, 1);
	
	lightss[1].pos_dir = vec4(10, 0,7, 0);
	lightss[1].color = vec4(1,0,1,0);
	lightss[1].attenuation = vec4(0, 0, 0.5, 1);

	if(x < width && y < height){
		vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
		vec4 tempColor = vec4(0);

		//Initialize the ray
		Ray r = initRay(x, y, camera);

		float t = FAR_CLIP, temp = FAR_CLIP;
		int currentObject = -1;
		int s = 1;
		//Check for intersection with an object in a brute force manner
		//No acceleration is implemented yet!(Octrees or kd-trees are possible);
		for(uint n = 0; n < reflectionDepth; ++n){
			for(int i = 0; i < numObjj; i++){
				switch(objectss[i].type){
					case 1:{
						temp = hitSphere(r, objectss[i].p);
						s = 0;
					} break;
									
					case 2:{
						temp = hitPlane(r, objectss[i].p);
					} break;
					
					case 3:{
						temp = hitTriangle(r, objectss[i].p);
					};
				}
				if(temp < t && temp >= -0.001f){
					t = temp;
					currentObject = i;
				}
			}

			if(currentObject != -1){
				tempColor = calculateColor(r, t, currentObject);
				if(tempColor != vec4(0)){
					if(materialss[objectss[currentObject].material_index].specularity != vec4(0)){
						color += materialss[objectss[currentObject].material_index].specularity * tempColor;
						r = getReflectionRay(r, currentObject, t);
						
						currentObject = -1;
						temp = t = FAR_CLIP;
					} else {
						color += tempColor;
						n = reflectionDepth;
					}
				} else {
					n = reflectionDepth;
				}
			} else {
				n = reflectionDepth;
			}
		}

		imageStore(outputTexture, ivec2(x, y), color);//vec4(s, 0, 0, 0));

	}
}