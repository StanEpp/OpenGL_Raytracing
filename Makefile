TARGET=executable/Raytracer.exe
DEBUG=1

OBJECTS=src/GLFWInput.o \
src/InputControl.o \
src/OpenGLRaytracer.o \
src/SceneLoader.o \
src/SceneReader.o \
src/ShaderManager.o \
src/Texture.o \
src/TextureRenderer.o \
src/XMLParser.o \
src/rply.o

CC=gcc
AR=ar

ifeq ($(DEBUG),1) 
	GDB = -ggdb -DDEBUG
else
	GDB = -DNDEBUG
endif

CPPFLAGS=-Iinclude/ -std=c++11
CFLAGS=-Iinclude/
LINKERFLAGS=-lglfw3 -lglew32 -lopengl32 -lgdi32 -lxerces-c -lstdc++ -std=c++11

all: src/main.cpp $(OBJECTS)
	$(CC) $(GDB) $(CPPFLAGS) -o $(TARGET) $< $(OBJECTS) $(LINKERFLAGS)

%.o: %.cpp
	$(CC) $(GDB) $(CPPFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(GDB) $(CFLAGS) -c $< -o $@
	
clean:
	rm $(subst /,\,$(OBJECTS))
	rm $(subst /,\,$(TARGET))