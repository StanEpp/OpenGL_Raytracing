This application is my first raytracer which uses OpenGL and its compute shader.
The whole naive raytracing algorithm is written in GLSL and can be found in the file "cs.glsl". 

[SCENE DESCRIPTION]
It is possible to create your own scenes using primitves like spheres, polygons and planes. 
For lightning you can use point lights and directional lights. 
I misused the .ply format as a scene descriptor. There you can define all the elements mentioned above
and place them in the scene. For further information you can take a look at the two test scenes 
"MirrorRoom.ply" and "CubeRoom.ply".

[LOADING SCENES]
To start the application and load a scene you only have to write the name of the scene file after the name
of the executable in your command prompt: "Raytracing.exe CubeRoom.ply"
It is possible to load more then one scene at once by placing further names at the end:
"Raytracing.exe CubeRoom.ply MirrorRoom.ply"

Thank you for downloading this humble project! :)
