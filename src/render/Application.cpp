// Include GLAD first
#include "glad/glad.h"
#include "Application.hpp"
#include "Scene.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>		// For std::cerr
#include <fstream>

Application::~Application() {
	glfwTerminate();
	exit(0);
}

int Application::Init() {
	LoadMap();
	InitializeGL();

	return 0;
}

void Application::LoadMap() {
	std::ifstream modelFile;

	scene = new Scene();
	scene->numVerts = 0;
	scene->numModels = 0;

	// Cube model
	modelFile.open("models/cube.txt");
	int numLines = 0;
	if(modelFile.is_open()) {
		modelFile >> numLines;
	}
	float *cubeModel = new float[numLines];
	for(int i = 0; i < numLines; i++) {
		modelFile >> cubeModel[i];
	}
	std::cout << "Cube model has: " << numLines << " lines" << std::endl;
	int numVertsCube = numLines/8;
	modelFile.close();

	// Key Model
	modelFile.open("models/knot.txt");
	numLines = 0;
	if(modelFile.is_open()) {
		modelFile >> numLines;
	}
	float *keyModel = new float[numLines];
	for(int i = 0; i < numLines; i++) {
		modelFile >> keyModel[i];
	}
	std::cout << "Knot model has: " << numLines << " lines" << std::endl;
	int numVertsKey = numLines/8;
	modelFile.close();

	scene->modelData = new float[8 * (numVertsCube + numVertsKey)];
	std::copy(cubeModel, cubeModel + 8 * numVertsCube, (scene->modelData));
	std::copy(keyModel, keyModel + 8 * numVertsKey, (scene->modelData) + 8 * numVertsCube);
	scene->numVerts = numVertsCube + numVertsKey;
	scene->startVerts[0] = 0;
	scene->startVerts[1] = numVertsCube;

	delete[] cubeModel;
	delete[] keyModel;
}

void Application::InitializeGL() {
	glfwInit();

	// Set window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create GLFW window
	m_window = glfwCreateWindow(m_spec.width, m_spec.height, m_spec.title, NULL, NULL);
	if(!m_window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(m_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}   

	glfwSwapInterval(1);

	// Set viewport
	int viewWidth, viewHeight;
	glfwGetFramebufferSize(m_window, &viewHeight, &viewHeight);
	glViewport(0, 0, viewWidth, viewHeight);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * (scene->numVerts) * sizeof(float), scene->modelData, GL_STATIC_DRAW);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, "", 0)

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
}

void Application::BeginRendering() {

}

void Application::RenderScene() {
	// Clear the frame
	glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	scene->deltaTime = glfwGetTime();

	glm::mat4 camera = glm::lookAt(
	glm::vec3(3.f, 0.f, 0.f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f)	);
	
	GLuint transformLoc = glGetUniformLocation();

	// Diplay the frame
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

int Application::Run() {
	while(!glfwWindowShouldClose(m_window)) {
		// Process input and events
		ProcessInput(m_window);

		BeginRendering();
		RenderScene();
	}

	scene->~Scene();

	std::cout << "passed" << std::endl;

	glfwTerminate();
	return 0;
}

// Process all input
void Application::ProcessInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)	// Close on escape
        glfwSetWindowShouldClose(window, true);
}