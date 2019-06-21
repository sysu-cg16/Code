#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>

#include <stb_image.h>
#include "sceneController.h"
#include "skyBox.h"

#include "ogldev_util.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool isFullScreen = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
float lightDir[] = { 0.0f, -0.3f, 0.0f };


SceneController sceneController;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWmonitor* pMonitor = isFullScreen ? glfwGetPrimaryMonitor() : NULL;
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project", pMonitor, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	vector<Spirit*> allCharacters;
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	

	Shader shader("animatedModel.vs", "animatedModel.fs");
	camera.MovementSpeed = 50.0f;
	sceneController.init();
	// now 
	//Spirit now_map("resources/now_map.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(20.0f, 20.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	//Spirit now_upper_half("resources/now_upper_half_v1.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(8.0f, 8.0f, 8.0f), glm::vec3(-90.0f, 0.0f, 0.0f));
	//Spirit now_lower_half("resources/now_lower_half_v1.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f), glm::vec3(0.0f, 180.0f, 0.0f));

	//allCharacters.push_back(&now_map);
	//allCharacters.push_back(&now_upper_half);
	//allCharacters.push_back(&now_lower_half);

	// Setup Dear ImGui context
	// ------------------------------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");


	SkyBox skyBox(&camera);
	skyBox.init();

	// render loop
// -----------
while (!glfwWindowShouldClose(window))
{
	// per-frame time logic
	// --------------------
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	const float RANGE_START = -400.0f;
	const float RANGE_END = 400.0f;

	// Start the Dear ImGui frame
	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplGlfw_NewFrame();
	//ImGui::NewFrame();

	//ImGui::Begin("Settting");
	//ImGui::SliderFloat3("lightDir", lightDir, -1, 1);
	//ImGui::SliderFloat3("planePos", (float*)&(sceneController.viewPlane->position), -10, 10);
	//ImGui::SliderFloat3("planeRota", (float*)&(sceneController.viewPlane->angles), 0, 360);
	////ImGui::SliderFloat3("planeRota", (float*)&(sceneController.viewPlane->angles), 0, 360);

	//ImGui::SliderFloat3("planeScale", (float*)&(sceneController.viewPlane->scale), 0, 10);
	//ImGui::SliderFloat3("blackPos", (float*)&(sceneController.forwardBlackHole->position), RANGE_START, RANGE_END);
	//ImGui::SliderFloat3("blackRota", (float*)&(sceneController.forwardBlackHole->angles), 0, 360);
	//ImGui::SliderFloat3("blackScale", (float*)&(sceneController.forwardBlackHole->scale), 0, 10);
	//ImGui::SliderFloat("sencer", &(sceneController.blackHoleSensitivity), RANGE_START, RANGE_END);
	//ImGui::End();

	// input
	// -----
	processInput(window);

	// render
	// ------
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// be sure to activate shader when setting uniforms/drawing objects
	shader.use();
	shader.setVec3("light.direction", lightDir[0], lightDir[1], lightDir[2]);
	shader.setVec3("viewPos", camera.Position);

	shader.setVec3("light.ambient", 0.4f, 0.4f, 0.4f);
	shader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);

	double time = glfwGetTime();

	sceneController.Draw(shader, time);
	skyBox.Draw();

	//ImGui::Render();
	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}


// glfw: terminate, clearing all previously allocated GLFW resources.
// ------------------------------------------------------------------
glfwTerminate();
return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
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

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		sceneController.viewPlane->position.x = camera.Position.x + 10 * camera.Front.x;
		sceneController.viewPlane->position.y = camera.Position.y + 10 * camera.Front.y;
		sceneController.viewPlane->position.z = camera.Position.z + 10 * camera.Front.z;
		sceneController.sceneChangeDetector();
}



	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
		glfwSetCursorPosCallback(window, NULL);
		glfwSetScrollCallback(window, NULL);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		firstMouse = true;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);

	sceneController.viewPlane->position.x = camera.Position.x + 10 * camera.Front.x;
	sceneController.viewPlane->position.y = camera.Position.y + 10 * camera.Front.y;
	sceneController.viewPlane->position.z = camera.Position.z + 10 * camera.Front.z;
	sceneController.viewPlane->angles2.x = camera.Pitch;
	sceneController.viewPlane->angles2.y = -90 - camera.Yaw;
	
	sceneController.sceneChangeDetector();
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}