# OpenGL Raytracer
This application is my first raytracer which uses OpenGL and its compute shader to perform naive raytracing.\
The whole naive raytracing algorithm is written in GLSL and can be found in the `src/cs.glsl` file.

## Build and Run
Clone the repository:
```
git clone https://github.com/StanEpp/OpenGL_Raytracing
```
Then build it:
```
cd OpenGL_Raytracing
mkdir build
cd build
cmake ..
make -j8
```
Execute the application and run one example scene:
```
./OpenGL_Raytracer scenes/CubeScene.obj
```
You can also load several scenes simultaneously:
```
./OpenGL_Raytracer scenes/CubeScene.obj scenes/MirrorRoom.obj
```

## Scene format
The application uses the [wavefront obj format](https://en.wikipedia.org/wiki/Wavefront_.obj_file) for scene descrpition. The library [objSpirit](https://github.com/stanepp/objspirit) loads the files. It extends the obj format by a few primitves and light source descriptors. Two example scenes are provided in the `scenes` folder.

## Keys
Mouse = Rotating the camera\
Numpad +/- = Increase/Decrease reflection depth\
W, A, S, D = Moving the camera