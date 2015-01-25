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
	int			*_oIndices, *_mIndices, *_lIndices, *_oOffsets, *_mOffsets, *_lOffsets;
	GLuint		_copyBufferIDs[2];
	GLenum		_props[1], _props2[1];

	void initialize(OpenGLRaytracer&);
	
public:
	SceneLoader() = delete;
	
	SceneLoader(OpenGLRaytracer& ogl) : 
		_numberOfObjInShader(0), _numberOfMaterialsInShader(0), _numberOfLightsInShader(0) {
		initialize(ogl);
	}
	
	~SceneLoader(){
		delete[] _oIndices;
		delete[] _mIndices;
		delete[] _lIndices;
		delete[] _oOffsets;
		delete[] _mOffsets;
		delete[] _lOffsets;
	}

	void loadScene(Scene&, OpenGLRaytracer&);
};

#endif