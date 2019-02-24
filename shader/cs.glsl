#version 430
#define FAR_CLIP 1000.0f

struct Camera{
    vec4 pos;
    vec4 dir;
    vec4 yAxis;
    vec4 xAxis;
    float tanFovY;
    float tanFovX;
};

//attenuation.w : 1 = Point light; 2 = Directional light
struct Light{
    vec4 pos_dir;
    vec4 color;
    vec4 attenuation;
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
    vec3 origin;
    vec3 dir;
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

uniform Camera camera;
uniform uint width;
uniform uint height;
uniform uint numObj;
uniform uint numLights;
uniform uint reflectionDepth;

writeonly uniform image2D outputTexture;

float hitSphere(Ray r, Primitive s)
{
    vec3 oc = r.origin - s.A.xyz;
    float s_roc = dot(r.dir, oc);
    float s_oc = dot(oc, oc);

    float d = s_roc * s_roc - s_oc + s.A.w * s.A.w;

    if(d < 0){
        return FAR_CLIP;
    } else {
        float t1 = sqrt(d);
        float t2 = -s_roc - t1;
        t1 = -s_roc + t1;

        // ray origin lies in the sphere
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

float hitPlane(Ray r, Primitive p)
{
    float s_nr = dot(p.B.xyz, r.dir);

    if(s_nr <= 0.00001f && s_nr >= -0.00001f){
        return FAR_CLIP;
    } else {
        float s_nv = dot(p.A.xyz, p.B.xyz);
        float s_no = dot(p.B.xyz, r.origin);

        return (s_nv - s_no) / s_nr;
    }

}

float hitTriangle(Ray r, Primitive t)
{
    vec3 AB = t.B.xyz - t.A.xyz;
    vec3 AC = t.C.xyz - t.A.xyz;
    mat3 mat = mat3(AB, AC, -1.0f * r.dir);

    float det = determinant(mat);

    if(det == 0.0f){
        return FAR_CLIP;
    } else {
        vec3 oA = r.origin - t.A.xyz;

        mat3 Di = inverse(mat);
        vec3 solution = Di * oA;

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

// Initialize the primary ray for pixel x, y
Ray initRay(uint x, uint y)
{
    float halfWidth = float(width) / 2.0f;
    float halfHeight = float(height) / 2.0f;

    float a = camera.tanFovX * ((float(x) - halfWidth + 0.5f) / halfWidth);
    float b = camera.tanFovY * ((halfHeight - float(y) - 0.5f) / halfHeight);

    vec3 dir = normalize((a * camera.xAxis + b * camera.yAxis + camera.dir).xyz);

    return Ray(camera.pos.xyz, dir);
}

Ray getReflectionRay(Ray r, float t, int objIdx)
{
    vec3 intersectionPoint = r.origin + r.dir * t;
    vec3 N = vec3(0);

    // Compute Normal at intersection point
    switch(objects[objIdx].type){
        // Sphere
        case 1:{
            N = normalize(intersectionPoint - objects[objIdx].p.A.xyz);
        } break;

        // Plane
        case 2:{
            N = normalize(objects[objIdx].p.B.xyz);
        } break;

        // Triangle
        case 3:{
            N = normalize(cross((objects[objIdx].p.B - objects[objIdx].p.A).xyz,
                                (objects[objIdx].p.C - objects[objIdx].p.A).xyz));
        } break;
    }

    // Compute reflected direction vector along the normal
    // r_dir   N   reflectedDir
    //     \   ^   ^
    //      \  |  /
    //       \ | /
    //        v|/
    vec3 reflectedDir = normalize(r.dir - 2 * dot(r.dir, N) * N);

    return Ray(intersectionPoint + reflectedDir * 0.01f, reflectedDir);
}

vec4 calculateColor(Ray r, float t, int objIdx)
{
    vec4 color = vec4(0);

    if (objIdx != -1) {
        vec3 N, L, H;
        int matIdx = objects[objIdx].material_index;
        vec3 intersectionPoint = r.origin + t * r.dir;

        switch (objects[objIdx].type) {
            case 1:{
                N = normalize(intersectionPoint - objects[objIdx].p.A.xyz);
            } break;

            case 2:{
                N = normalize(objects[objIdx].p.B.xyz);

                //Mirror the normal if the camera's position is at the other side of the plane.
                //This avoids considering light sources behind the plane.
                if( dot(N, camera.pos.xyz - intersectionPoint) < 0){
                    N = -1.f * N;
                }
            } break;

            case 3:{
                N = normalize(cross((objects[objIdx].p.B - objects[objIdx].p.A).xyz,
                                    (objects[objIdx].p.C - objects[objIdx].p.A).xyz));
            } break;
        }

        intersectionPoint += 0.01f * N;

        for (int j = 0; j < numLights; ++j) {
            int lightType = int(lights[j].attenuation.w);

            switch (lightType) {
                //Point light
                case 1:{
                    L = normalize(lights[j].pos_dir.xyz - intersectionPoint);
                }break;

                //Directional light
                case 2:{
                    L = normalize(lights[j].pos_dir.xyz);
                }break;
            }

            Ray shadowRay = Ray(intersectionPoint, L);
            bool inShadow = false;

            for (int i = 0; i < numObj; ++i) {
                float temp = FAR_CLIP;

                switch (objects[i].type) {
                    case 1:{
                        temp = hitSphere(shadowRay, objects[i].p);
                    } break;

                    case 2:{
                        temp = hitPlane(shadowRay, objects[i].p);
                    } break;

                    case 3:{
                        temp = hitTriangle(shadowRay, objects[i].p);
                    } break;
                }

                switch (lightType) {
                    //Point light
                    case 1:{
                        if (temp < FAR_CLIP &&
                            temp >= -0.001f &&
                            temp < length(intersectionPoint - lights[j].pos_dir.xyz))
                        {
                            inShadow = true;
                            i = int(numObj);
                        }
                    } break;

                    //Directional light
                    case 2:{
                        if (temp < FAR_CLIP && temp >= -0.001f) {
                            inShadow = true;
                            i = int(numObj);
                        }
                    } break;
                }

            }

            if (!inShadow) {
                H = normalize(L + normalize(camera.pos.xyz - intersectionPoint));

                if (dot(N, L) > 0) {
                    float dist = length(lights[j].pos_dir.xyz - intersectionPoint);
                    vec3 attCoef = lights[j].color.xyz / (lights[j].attenuation.x +
                                   lights[j].attenuation.y * dist +
                                   lights[j].attenuation.z * dist * dist * 0.01f);

                    // Check if the object has a material.
                    // If not use default values to compute color.
                    if (matIdx != -1) {
                        vec3 phong = materials[matIdx].diffuse.xyz * dot(N, L) +
                                     materials[matIdx].specularity.xyz * pow(max(dot(-N,H), 0), materials[matIdx].shininess);
                        color += vec4(attCoef * phong, 0.0f);
                    } else {
                        vec3 phong = vec3(0.5,0.5,0.5) * max(dot(N, L), 0) +
                                     vec3(0.5,0.5,0.5) * pow( max( dot(N,H), 0), 10);
                        color += vec4(attCoef * phong, 0.0f);
                    }
                }
            }
        }

        if (matIdx != -1) {
            color += materials[matIdx].emission;
        }
    }

    return color;
}

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
void main()
{
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;

    if (x < width && y < height) {
        vec4 color = vec4(0.f);

        // Initialize the primary ray
        Ray r = initRay(x, y);

        // Check for intersection with an object in a brute force manner
        // No acceleration is implemented!
        for (uint n = 0; n < reflectionDepth + 1; ++n) {

            int objIdx = -1;
            float t = FAR_CLIP, temp = FAR_CLIP;

            // Check for closest intersection of ray r and an object
            for(int i = 0; i < numObj; i++){
                switch(objects[i].type){
                    case 1:{
                        temp = hitSphere(r, objects[i].p);
                    } break;

                    case 2:{
                        temp = hitPlane(r, objects[i].p);
                    } break;

                    case 3:{
                        temp = hitTriangle(r, objects[i].p);
                    };
                }
                if(temp < t && temp >= -0.001f){
                    t = temp;
                    objIdx = i;
                }
            }

            // Check if the ray hit an object.
            // If so compute Color at intersection otherwise exit early.
            if (objIdx != -1) {
                vec4 tempColor = calculateColor(r, t, objIdx);
                vec4 spec = materials[objects[objIdx].material_index].specularity;

                // Check if material specularity is defined (i.e. spec != vec4(0))
                // If so compute reflection ray and continue loop otherwise exit early.
                if (dot(spec, spec) > 0.001){
                    color += spec * tempColor;
                    r = getReflectionRay(r, t, objIdx);
                } else {
                    color += tempColor;
                    break;
                }
            } else {
                break;
            }
        }

        imageStore(outputTexture, ivec2(x, y), color);
    }
}