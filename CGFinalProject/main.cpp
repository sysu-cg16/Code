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

//#define IMGUI_TEST

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

void changePlanePos();
void changePlaneInitAng(float xoffset, float yoffset, bool reset = false);
void showGui();
void getDepthMap(Shader &depthShader, float &currentFrame, glm::mat4 &lightSpaceMatrix);
void showScence(Shader &shader, float &currentFrame, glm::mat4 &lightSpaceMatrix);
void showParticle(Shader &shader);
void showDepthMap(Shader &debugDepthQuad);
void renderQuad();

// camera
Camera camera(glm::vec3(0.0f, 50.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool isFullScreen = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
float lightPos[] = { 32.0f, 423.0f, 285.0f };
float lightPan = 800;

//float lightPos[] = { 57.0f, 262.0f, 141.0f };
//float lightPan = 550;

float defaultViewPlaneInitAng[] = { 35.f, 0.0f, 0.0f };
glm::vec3 viewPlaneInitAng(defaultViewPlaneInitAng[0], defaultViewPlaneInitAng[1], defaultViewPlaneInitAng[2]);

// gamma
bool gammaEnabled = false;
bool gammaKeyPressed = false;

// depth test
bool isDepthTest = false;
bool depthTestKeyPressed = false;

//粒子发射器
ParticleGenerator   *Particles;

SceneController sceneController;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

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

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	// 文字开启混合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
	// build and compile our shader zprogram
	// ------------------------------------
	Shader shader("animatedModel.vs", "animatedModel.fs");
	Shader depthShader("shadow_mapping_depth.vs", "shadow_mapping_depth.fs");
	Shader debugDepthQuad("debug_shadow_mapping.vs", "debug_shadow_mapping.fs");
	Shader particleShader("particle.vs", "particle.fs");

	//粒子发射器
	Particles = new ParticleGenerator(
		particleShader,
		loadTexture("resources/particle.png"),
		500
	);

	
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

	// init variable
	// ------------------------------
	camera.MovementSpeed = 100.0f;
	sceneController.init();
	SkyBox skyBox(&camera);
	skyBox.init();

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		glfwGetWindowSize(window, (int*)&SCR_WIDTH, (int*)&SCR_HEIGHT);
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		const float RANGE_START = -400.0f;
		const float RANGE_END = 400.0f;

		// input
		// -----
		processInput(window);
		changePlaneInitAng(0, 0, true);
		changePlanePos();
		sceneController.sceneChangeDetector();

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//get lightSpaceMatrix
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		lightProjection = glm::ortho(-lightPan, lightPan, -lightPan, lightPan, 0.1f, lightPan);
		lightView = glm::lookAt(glm::vec3(lightPos[0], lightPos[1], lightPos[2]),
								glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		getDepthMap(depthShader, currentFrame, lightSpaceMatrix);
		
		skyBox.Draw();
		// 2. render scene as normal using the generated depth/shadow map  
		// --------------------------------------------------------------
		showScence(shader, currentFrame, lightSpaceMatrix);

		showParticle(particleShader);

		skyBox.Draw();

		if (isDepthTest) showDepthMap(debugDepthQuad);
    
 #ifdef IMGUI_TEST
		showGui();
 #endif // IMGUI_TEST

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

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
		sceneController.setThisFramePressed('W');
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		sceneController.setThisFramePressed('S');
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
		sceneController.setThisFramePressed('A');
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
		sceneController.setThisFramePressed('D');
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
	}

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && !gammaKeyPressed)
	{
		gammaEnabled = !gammaEnabled;
		gammaKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
	{
		gammaKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !depthTestKeyPressed)
	{
		isDepthTest = !isDepthTest;
		depthTestKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
	{
		depthTestKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		glEnable(GL_MULTISAMPLE);
	}
	
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		glDisable(GL_MULTISAMPLE);
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

	changePlaneInitAng(xoffset, yoffset);
	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void showGui() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settting");
	ImGui::SliderFloat3("lightPos", lightPos, -500, 500);
	//ImGui::InputFloat3("lightPos", lightPos, 2);
	ImGui::InputFloat("lightPan", &lightPan, 2);
	//ImGui::SliderFloat3("planePos", (float*)&(sceneController.viewPlane->position), -10, 10);
	ImGui::SliderFloat3("planeRota", (float*)&(sceneController.viewPlane->angles), 0, 360);
	ImGui::SliderFloat3("planeRota", (float*)&(viewPlaneInitAng), 0, 360);
	
	/*ImGui::SliderFloat3("planeScale", (float*)&(sceneController.viewPlane->scale), 0, 10);
	ImGui::SliderFloat3("blackPos", (float*)&(sceneController.forwardBlackHole->position), RANGE_START, RANGE_END);
	ImGui::SliderFloat3("blackRota", (float*)&(sceneController.forwardBlackHole->angles), 0, 360);
	ImGui::SliderFloat3("blackScale", (float*)&(sceneController.forwardBlackHole->scale), 0, 10);
	ImGui::SliderFloat("sencer", &(sceneController.blackHoleSensitivity), RANGE_START, RANGE_END);*/
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void changePlanePos() {
	sceneController.viewPlane->position.x = camera.Position.x + 10 * camera.Front.x;
	sceneController.viewPlane->position.y = camera.Position.y + 10 * camera.Front.y;
	sceneController.viewPlane->position.z = camera.Position.z + 10 * camera.Front.z;
	sceneController.viewPlane->angles.x = viewPlaneInitAng.x + camera.Pitch;
	sceneController.viewPlane->angles.y = viewPlaneInitAng.y - 90 - camera.Yaw;
}

void changePlaneInitAng(float xoffset, float yoffset, bool reset) {
	if (reset) {
		if (!isFloatEqual(viewPlaneInitAng.x, defaultViewPlaneInitAng[0]))
			viewPlaneInitAng.x += viewPlaneInitAng.x < defaultViewPlaneInitAng[0] ? 1 : -1;
		if (!isFloatEqual(viewPlaneInitAng.y, defaultViewPlaneInitAng[1]))
			viewPlaneInitAng.y += viewPlaneInitAng.y < defaultViewPlaneInitAng[1] ? 1 : -1;
	}

	else {
		if (yoffset < 0) {
			if (viewPlaneInitAng.x > defaultViewPlaneInitAng[0] - 25.0f)
				viewPlaneInitAng.x -= 2;
		}
		else if (yoffset > 0) {
			if (viewPlaneInitAng.x < defaultViewPlaneInitAng[0] + 15.0f)
				viewPlaneInitAng.x += 2;
		}

		if (xoffset > 0) {
			if (viewPlaneInitAng.y > defaultViewPlaneInitAng[1]- 25.0f)
				viewPlaneInitAng.y -= 2;
		}
		else if (xoffset < 0) {
			if (viewPlaneInitAng.y < defaultViewPlaneInitAng[1] + 25.0f)
				viewPlaneInitAng.y += 2;
		}
	}
}

void getDepthMap(Shader &depthShader, float &currentFrame, glm::mat4 &lightSpaceMatrix) {
	depthShader.use();
	depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, sceneController.depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	sceneController.Draw(depthShader, currentFrame);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void showScence(Shader &shader, float &currentFrame, glm::mat4 &lightSpaceMatrix) {
	shader.use();
	shader.setInt("gamma", gammaEnabled);
	shader.setInt("shadowMap", 0);
	shader.setVec3("light.direction", -lightPos[0], -lightPos[1], -lightPos[2]);
	shader.setVec3("viewPos", camera.Position);

	shader.setVec3("light.ambient", 0.5f, 0.5f, 0.5f);
	shader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneController.depthMap);
	sceneController.Draw(shader, currentFrame);

	//FontRender::getInstance()->RenderCharacter('W', 25.0f, 25.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
}

void showDepthMap(Shader &debugDepthQuad) {
	debugDepthQuad.use();
	debugDepthQuad.setInt("depthMap", 0);
	debugDepthQuad.setFloat("near_plane", 0.1f);
	debugDepthQuad.setFloat("far_plane", lightPan);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sceneController.depthMap);
	renderQuad();
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void showParticle(Shader &shader) {
	shader.use();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setFloat("scale", 10.0f);
	if (sceneController.isForwardShow) {
		Particles->Update(0.01, *(sceneController.forwardBlackHole), 5, glm::vec3(0.0f));
	}
	else if (sceneController.isBackwardShow) {
		Particles->Update(0.01, *(sceneController.backwardBlackHole), 5, glm::vec3(0.0f));
	}
	Particles->Draw();
}
