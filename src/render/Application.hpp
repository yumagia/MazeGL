#ifndef APPLICATION_INCLUDED
#define APPLICATION_INCLUDED

#include <GLFW/glfw3.h>
#include "Scene.hpp"
#include <glm/glm.hpp>

struct ApplicationSpecification {
	int width = 1200;
	int height = 900;
	const char *title = "Maze";
};

class Application {
public:
	Application() {}
	~Application();

	int Init();

	int Run();

private: 
	void InitializeGL();
	void LoadMap();


	void BeginRendering();
	void RenderScene();
	void DrawModel(int startVert, int NumVerts, glm::vec3 pos, glm::mat4 rotatMat, glm::vec3 color);

	void ProcessInput(GLFWwindow *window);
	
private:

	ApplicationSpecification m_spec;
	GLFWwindow *m_window;

	Scene *scene;
	GLuint shaderHandle;

	GLuint vbo;
	GLuint vao;
};

#endif