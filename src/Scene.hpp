#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED

#define MAX_SCENE_MODELS	5

class Scene {
public:
	Scene() {}
	~Scene();

	float	*modelData = nullptr;
	int		numVerts = 0;
	int		startVerts[MAX_SCENE_MODELS];
	int		numModels = 0;
};

#endif