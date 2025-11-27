#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED

#define MAX_SCENE_MODELS 5

#include "Math.hpp"

struct Player {
	Vec3f	origin;
	Vec3f	vel;

	Vec3f	wishDir;

	float	yaw;
	float	pitch;
};

class Scene {
public:
	Scene() {}
	~Scene();

	float	*modelData = nullptr;
	int		numVerts = 0;
	int		startVerts[MAX_SCENE_MODELS];
	int		numModels = 0;

	int		width, height;

	int		*level = nullptr;

	float deltaTime = 0;

	Player player;
};

#endif