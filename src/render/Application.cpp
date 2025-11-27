// Include GLAD first
#include "glad/glad.h"
#include "Application.hpp"
#include "Scene.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>		// For std::cerr
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include "Math.hpp"

enum {
	LEVEL_AIR,
	LEVEL_SPAWN,
	LEVEL_WALL,
	LEVEL_GOAL,
	LEVEL_KEY,
	LEVEL_DOOR
};

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

	std::ifstream mapFile;
	mapFile.open("scenefiles/no_doors.txt");

	if(!mapFile) {
		std::cerr << "error opening mapfile!" << std::endl;
	}

	std::string line;
	std::getline(mapFile, line);
	scene->width = line[0] - '0';
	scene->height = line[2] - '0';

	scene->level = new int[scene->width * scene->height];

	int gridCount = 0;
	for(int i = 0; i < scene->height; i++) {
		std::getline(mapFile, line);
		for(int j = 0; j < scene->width; j++) {
			if(line[j] == '0') {
				scene->level[gridCount] = LEVEL_AIR;
			}
			else if(line[j] == 'W') {
				scene->level[gridCount] = LEVEL_WALL;
			}
			else if(line[j] == 'S') {
				scene->level[gridCount] = LEVEL_AIR;
				scene->player.origin = Vec3f(j, 0, i);
			}
			else if(line[j] == 'G') {
				scene->level[gridCount] = LEVEL_GOAL;
			}

			gridCount++;
		}
	}

	mapFile.close();
}

std::string readShaderSource(const char *fileName) {
	std::ifstream shaderFile;

	shaderFile.open(fileName);
	if(!shaderFile.is_open()) {
		std::cerr << "Cannot open shader source file" << std::endl;
		return NULL;
	}

	std::stringstream buffer;
	buffer << shaderFile.rdbuf();

	std::string fileContents = buffer.str();

	return fileContents;
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
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * (scene->numVerts) * sizeof(float), scene->modelData, GL_STATIC_DRAW);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertexSourceString = readShaderSource("vertex.glsl");
	const GLchar *vertexSource = (vertexSourceString.c_str());
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	std::string fragmentSourceString = readShaderSource("fragment.glsl");
	const GLchar *fragmentSource = (fragmentSourceString.c_str());
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	// Create the shader program
	shaderHandle = glCreateProgram();

	glAttachShader(shaderHandle, vertexShader);
	glAttachShader(shaderHandle, fragmentShader);

	glLinkProgram(shaderHandle); 

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Tell OpenGL how to set fragment shader input
	GLint posAttrib = glGetAttribLocation(shaderHandle, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(posAttrib);

	GLint normAttrib = glGetAttribLocation(shaderHandle, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
	glEnableVertexAttribArray(normAttrib);
	
	GLint texAttrib = glGetAttribLocation(shaderHandle, "inTexcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

	GLint uniView = glGetUniformLocation(shaderHandle, "view");
	GLint uniProj = glGetUniformLocation(shaderHandle, "proj");

	glBindVertexArray(0); //Unbind the VAO once we have set all the attributes

	glEnable(GL_DEPTH_TEST);
}

void Application::BeginRendering() {
	// Clear the frame
	glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderHandle);

	scene->deltaTime = glfwGetTime();

	glm::mat4 view = glm::lookAt(
	glm::vec3(5.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f)	);
	
	glm::mat4(1);

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), m_spec.width / (float) m_spec.height, 1.0f, 10.0f);
	
	GLuint transformView = glGetUniformLocation(shaderHandle, "view");
	glUniformMatrix4fv(transformView, 1, GL_FALSE, glm::value_ptr(view));

	GLuint transformProj = glGetUniformLocation(shaderHandle, "proj");
	glUniformMatrix4fv(transformProj, 1, GL_FALSE, glm::value_ptr(proj));

	glBindVertexArray(vao);
}

void Application::RenderScene() {
	BeginRendering();

	int gridCount = 0;
	for(int y = 0; y < scene->height; y++) {
		for(int x = 0; x < scene->width; x++) {
			if(scene->level[gridCount] == LEVEL_WALL) {
				DrawModel(scene->startVerts[0], scene->startVerts[1], glm::vec3(x, y, 0), glm::mat4(1), glm::vec3(1.f, 1.f, 1.f));
			}

			DrawModel(scene->startVerts[0], scene->startVerts[1], glm::vec3(x, y, -1), glm::mat4(1), glm::vec3(0.1f, 0.1f, 0.1f));
			gridCount++;
		}
	}
}

void Application::DrawModel(int startVert, int NumVerts, glm::vec3 pos, glm::mat4 rotatMat, glm::vec3 color) {
	GLint uniColor = glGetUniformLocation(shaderHandle, "inColor");
	glUniform3fv(uniColor, 1, glm::value_ptr(color));
    GLint uniTexID = glGetUniformLocation(shaderHandle, "texID");

	glm::mat4 model = glm::mat4(1);
	
	model = glm::translate(model, pos);
	GLint uniModel = glGetUniformLocation(shaderHandle, "model");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(uniTexID, -1);
	
	glDrawArrays(GL_TRIANGLES, startVert, NumVerts);
}

int Application::Run() {
	while(!glfwWindowShouldClose(m_window)) {
		// Process input and events
		ProcessInput(m_window);

		RenderScene();

		// glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		// glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Diplay the frame
		glfwSwapBuffers(m_window);

		// Poll IO
		glfwPollEvents();
	}

	// De-allocations
	scene->~Scene();

	glDeleteProgram(shaderHandle);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	glfwTerminate();

	std::cout << "Window sucessfully closed" << std::endl;

	return 0;
}

// Process all input
void Application::ProcessInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {	// Close on escape
        glfwSetWindowShouldClose(window, true);
	}
}