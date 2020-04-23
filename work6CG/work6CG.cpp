#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

#include "Shader.h"

using namespace glm;

vec3 forward(1.0f, 0.0, 0.0f);
vec3 right(0.0f, 0.0, 1.0f);
vec3 processKeys(GLFWwindow* window , float dt) {
	vec3 result(0.0f);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		result += forward;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		result -= forward;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		result -= right;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		result += right;
	}

	return result * dt;
}

int main(void)
{
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	float vertices[] = {
		-0.5f,0.0f,0.0f,
		0.5f,0.0f,0.0f,
		0.0f,0.0f,-0.5f,

		-0.2f,0.4f,0.0f,
		0.2f,0.4f,0.0f,
		0.0f,0.4f,-0.2f,
	};

	unsigned indices[] = {
		0,1,2,
		0,3,1,
		3,4,1,
		1,4,5,
		1,5,2,
		2,5,3,
		2,3,0
	};

	unsigned indicesLine[] = {
		1,0,
		1,2,
		2,0,
		0,3,
		1,4,
		5,2,
		3,4,
		4,5,
		3,5
	};

	unsigned vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned ib;
	glGenBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	unsigned ibLines;
	glGenBuffers(1, &ibLines);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibLines);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesLine), indicesLine, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


	//line smoothing and blending
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glLineWidth(3.0f);
	glfwWindowHint(GLFW_SAMPLES, 4);

	Shader basic("basic.shader");

	vec3 renderColor(1.0f, 0.5f,0.0f);
	vec3 lineColor(0, 0, 0);


	vec3 figurePos(0.0f);
	vec3 from(1.0f, 1.0f, 1.0f);
	vec3 to(0.0f);
	vec3 up(0.0f, 1.0f, 0.0f);
	mat4 model(1.0f);

	
	mat4 view = lookAt(from, to - from, up);
	mat4 proj = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	basic.setUniform4m("u_model", model);
	basic.setUniform4m("u_view", view);
	basic.setUniform4m("u_proj", proj);
	basic.setUniformVec3("renderColor", renderColor);

	float lastTime = glfwGetTime();
	float deltaTime = 0;

	float timerTime = 5.0f;
	float start;
	//Setup IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();


	bool isRotating = false;
	while (!glfwWindowShouldClose(window))
	{
		float tTime = glfwGetTime();
		deltaTime = lastTime - tTime;

		lastTime = tTime;

		glEnable(GL_DEPTH_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		ImGui_ImplOpenGL3_NewFrame();

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		if (ImGui::Button("Press to rotate")) {
			isRotating = true;
			start = tTime;
		}

		if (isRotating) {

			float timeToGo = tTime - start;

			if (timeToGo > timerTime) {
				isRotating = false;
			}
			mat4 rot = rotate(mat4(1.0f), (float)cos(glfwGetTime()) * 10,vec3(1.0f, 0.0, 0.0f));
			rot = rotate(rot, (float)sin(radians(glfwGetTime())) * 10, vec3(0.0f, 0.0f, 1.0f));

			
			basic.setUniform4m("u_model", rot);

		}
		else {
			figurePos += processKeys(window, deltaTime);
			mat4 rotateX = rotate(mat4(1.0f), figurePos.x, vec3(1.0f, 0.0, 0.0f));
			mat4 rotateZ = rotate(mat4(1.0f), figurePos.z, vec3(0.0f, 0.0f, 1.0f));
			mat4 res = rotateX * rotateZ;
			basic.setUniform4m("u_model", res);
		}

	
		//render mesh
		basic.setUniformVec3("renderColor", renderColor);
		basic.bind();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glDrawElements(GL_TRIANGLES, 21, GL_UNSIGNED_INT, 0);
		//render lines above the mesh
		basic.setUniformVec3("renderColor", lineColor);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibLines);
		basic.bind();
		glDrawElements(GL_LINES, 18, GL_UNSIGNED_INT, 0);


		//Render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Shutdown
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}