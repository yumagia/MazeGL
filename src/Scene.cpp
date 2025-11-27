#include "Scene.hpp"
#include <iostream>

Scene::~Scene() {
	delete[] modelData;
	modelData = nullptr;
	delete[] level;
	level = nullptr;
}