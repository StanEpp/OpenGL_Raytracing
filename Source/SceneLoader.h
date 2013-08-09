#ifndef _SCENELOADER_
#define _SCENELOADER_

#include "OpenGLRaytracer.h"
#include "Primitives.h"
#include "Scene.h"

class SceneLoader{
private:
	int			_numberOfObjInShader, _numberOfMaterialsInShader, _numberOfLightsInShader;
	int			_materialElements, _objectElements, _lightElements;
	int			_oBlockSize, _oBlockIndex, _mBlockSize, _mBlockIndex, _lBlockIndex, _lBlockSize;
	int			_oAlignOffset, _mAlignOffset, _lAlignOffset;
	int*		_oIndices, *_mIndices, *_lIndices, *_oOffsets, *_mOffsets, *_lOffsets;
	GLuint*		_copyBufferIDs;
	GLenum		_props[1], _props2[1];

	void initialize(OpenGLRaytracer&);
	
public:
	SceneLoader(OpenGLRaytracer& ogl){
		_numberOfObjInShader = 0;
		_numberOfMaterialsInShader = 0;
		_numberOfLightsInShader = 0;
		_copyBufferIDs = new GLuint[2];
		initialize(ogl);
	}
	~SceneLoader(){
		delete _oIndices;
		delete _mIndices;
		delete _oOffsets;
		delete _mOffsets;
		delete _lIndices;
		delete _lOffsets;
		delete _copyBufferIDs;
	}

	void test(OpenGLRaytracer& ogl);
	void loadScene(Scene&, OpenGLRaytracer&);
};

#endif