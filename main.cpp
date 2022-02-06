#include <iostream>
#include "shader.h"
#include <camera.h>
#include <algorithm>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#define GLM_SWIZZLE

#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"glfw3.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
std::vector<int> randperm(int Num);
void changeStripe();
void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// stripe color
Shader columnShader;
Shader rowShader;

const int stripe_num = 10;
int rowRandom[stripe_num] = { 0, 10, 4, 7 };
int colRandom[stripe_num] = { 0, 10, 4, 7 };
bool canScale[stripe_num] = {0}; // 控制条纹的缩放

float my_color_1[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float my_color_2[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
float my_color_3[4] = { 228.f, 145.f, 0.f, 0.0f };

glm::vec3 setColor1 = glm::vec3(my_color_1[0], my_color_1[1], my_color_1[2]);
glm::vec3 setColor2 = glm::vec3(my_color_2[0], my_color_2[1], my_color_2[2]);
glm::vec3 setColor3 = glm::vec3(my_color_3[0], my_color_3[1], my_color_3[2]);

int main() {
	
	glfwInit();
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
    // --------------------
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Lattice-Generator", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, onKeyPress);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Compiles shaders.
	columnShader = Shader("column.vs", "column.fs"); // 列
	rowShader = Shader("row.vs", "row.fs"); // 行

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	GLfloat vertices[] = {
		// column				
		-0.95f,  1.0f, 0.0f,  // Top Right
		-0.95f, -1.0f, 0.0f,  // Bottom Right
		-1.0f, -1.0f, 0.0f, // Bottom Left
		-1.0f,  1.0f, 0.0f  // Top Left 
	};
	GLfloat vertices2[] = {
		// row
		 1.0f, -0.95f, 0.0f,  // Top Right
		 1.0f, -1.0f, 0.0f,  // Bottom Right
		-1.0f,  -1.0f, 0.0f,// Bottom Left
		-1.0f, -0.95f, 0.0f// Top Left 
	};
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3,  // First Triangle
		1, 2, 3   // Second Triangle
	};

	// positions all containers
	glm::vec3 columnPositions[] = {
	glm::vec3(0.1f,  0.0f, 0.0f),
	glm::vec3(0.2f,  0.0f, 0.0f),
	glm::vec3(0.3f,  0.0f, 0.0f),
	glm::vec3(0.4f,  0.0f, 0.0f),
	glm::vec3(0.5f,  0.0f, 0.0f),
	glm::vec3(0.6f,  0.0f, 0.0f),
	glm::vec3(0.7f,  0.0f, 0.0f),
	glm::vec3(0.8f,  0.0f, 0.0f),
	glm::vec3(0.9f,  0.0f, 0.0f),
	glm::vec3(1.0f,  0.0f, 0.0f),
	glm::vec3(1.1f,  0.0f, 0.0f),
	glm::vec3(1.2f,  0.0f, 0.0f),
	glm::vec3(1.3f,  0.0f, 0.0f),
	glm::vec3(1.4f,  0.0f, 0.0f),
	glm::vec3(1.5f,  0.0f, 0.0f),
	glm::vec3(1.6f,  0.0f, 0.0f),
	glm::vec3(1.7f,  0.0f, 0.0f),
	glm::vec3(1.8f,  0.0f, 0.0f)
	};

	// configure columnVAO
	columnShader.use();
	unsigned int columnVBO, columnVAO, EBO;
	glGenVertexArrays(1, &columnVAO);
	glGenBuffers(1, &columnVBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(columnVAO);
	glBindBuffer(GL_ARRAY_BUFFER, columnVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// configure rowcolumnVAO
	rowShader.use();
	unsigned int rowVBO, rowVAO;
	glGenVertexArrays(1, &rowVAO);
	glGenBuffers(1, &rowVBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(rowVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rowVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// set color blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	changeStripe();

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Text("Lattice-Generator");
		// Edit a color (stored as ~4 floats)
		if (ImGui::ColorEdit4("Color1", my_color_1))
		{
			columnShader.use();
			setColor1 = glm::vec3(my_color_1[0], my_color_1[1], my_color_1[2]);
			columnShader.setVec3("setColor", setColor1);
		}
		if (ImGui::ColorEdit4("Color2", my_color_2))
		{
			setColor2 = glm::vec3(my_color_2[0], my_color_2[1], my_color_2[2]);
			rowShader.use();
			rowShader.setVec3("setColor", setColor2);
		}
		if (ImGui::ColorEdit4("Color3", my_color_3))
		{
			glClearColor(my_color_3[0], my_color_3[1], my_color_3[2], 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		// Rendering
		ImGui::Render();

		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);
		
		// render background
		glClearColor(my_color_3[0], my_color_3[1], my_color_3[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render the cube

		// draw col
		columnShader.use();
		glBindVertexArray(columnVAO);
		glm::mat4 col_view = camera.GetViewMatrix();
		columnShader.setMat4("view", col_view);
		glm::mat4 col_model = glm::mat4(1.0f);
		columnShader.setMat4("model", col_model);
		for (unsigned int i = 0; i < stripe_num; i++)
		{
			columnShader.use();
			columnShader.setVec3("setColor", setColor1);
			glm::mat4 col_model;
			// 先缩放再平移
			if (canScale[i]) {
				columnShader.setVec3("setColor", setColor2);
				col_model = glm::scale(col_model, glm::vec3(2.0f, 1.0f, 1.0f));
			}
			col_model = glm::translate(col_model, columnPositions[colRandom[i]]);
			columnShader.setMat4("model", col_model);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		// draw row
		rowShader.use();
		glBindVertexArray(rowVAO);
		glm::mat4 row_view = camera.GetViewMatrix();
		rowShader.setMat4("view", row_view);
		glm::mat4 row_model = glm::mat4(1.0f);
		rowShader.setMat4("model", row_model);
		for (unsigned int i = 0; i < stripe_num; i++)
		{
			rowShader.use();
			rowShader.setVec3("setColor", setColor1);
			glm::vec3 _rowPosition = columnPositions[rowRandom[i]];
			float x = _rowPosition.x;
			float y = _rowPosition.y;
			float z = _rowPosition.z;
			glm::vec3 rowPosition = glm::vec3(y, x, z);
			glm::mat4 row_model;
			if (canScale[i]) {
				rowShader.setVec3("setColor", setColor2);
				row_model = glm::scale(row_model, glm::vec3(1.0f, 2.0f, 1.0f));
			}
			row_model = glm::translate(row_model, rowPosition);
			rowShader.setMat4("model", row_model);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	// Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// 工具函数：返回随机的不重复数组
std::vector<int> randperm(int Num)
{
	std::vector<int> temp;
	for (int i = 0; i < Num; ++i)
	{
		temp.push_back(i + 1);
	}
	random_shuffle(temp.begin(), temp.end());
	return temp;
}

void changeColor() {
	/*std::cout << "Change color" << std::endl;*/
	my_color_1[0] = (float)(rand() % 255) / 255;
	my_color_1[1] = (float)(rand() % 255) / 255;
	my_color_1[2] = (float)(rand() % 255) / 255;
	
	my_color_2[0] = (float)(rand() % 255) / 255;
	my_color_2[0] = (float)(rand() % 255) / 255;
	my_color_2[0] = (float)(rand() % 255) / 255;

	my_color_3[0] = (float)(rand() % 255) / 255;
	my_color_3[1] = (float)(rand() % 255) / 255;
	my_color_3[2] = (float)(rand() % 255) / 255;

	// set random color
	setColor1 = glm::vec3(my_color_1[0], my_color_1[1], my_color_1[2]);
	setColor2 = glm::vec3(my_color_2[0], my_color_2[1], my_color_2[2]);
	setColor3 = glm::vec3(my_color_3[0], my_color_3[1], my_color_3[2]);
}

void changeStripe() {
	//std::cout << "KEY_SPACE press once" << std::endl;
	std::vector<int> temp = randperm(18);
	for (int i = 0; i < stripe_num; i++) {
		rowRandom[i] = temp[i];
		colRandom[i] = temp[i];
	}
	// changecolor
	changeColor();
	// if scale
	for (int i = 0; i < stripe_num; i++) {
		canScale[i] = rand() % 2;
	}
}

// 按键的 state 和 event 是分开处理的

void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action != GLFW_RELEASE)
	{
		changeStripe();
	}
}