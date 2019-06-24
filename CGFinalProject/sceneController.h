#ifndef SCENE_CONTROLLER__H
#define SCENE_CONTROLLER__H

#include "scene.h"
#include <vector>
#include "fontRender.h"
#include "particle_generator.h"

class SceneController
{
DISALLOW_COPY_AND_ASSIGN(SceneController)
public:
	SceneController();
	~SceneController();
	void Draw(Shader shader, float time);
	void init();
	float blackHoleSensitivity;
	Spirit* forwardBlackHole;
	Spirit* backwardBlackHole;
	Spirit* viewPlane;
	void sceneChangeDetector();
	void setThisFramePressed(const char pressed);
	unsigned int depthMapFBO;
	unsigned int depthMap;
	bool isForwardShow;
	bool isBackwardShow;

private:
	void initDepthMapFBO();
	void initScenePast();
	void initSceneNow();
	vector<Scene*> allScenes;
	int sceneIndex;
	bool blackHoleDistancePreEstimate(const glm::vec3& holePos) const;

	// 用于当前按钮显示
	FontRender* fontRender;
	char thisFramePressed;
	bool isPressedThisFrame;
	unsigned int pressedCount;
};

inline void SceneController::init()
{
	isPressedThisFrame = false;
	fontRender = FontRender::getInstance();
	forwardBlackHole = new Spirit("BlackHole.fbx", glm::vec3(-50.0f,250.0f, -50.0f), glm::vec3(10.0f, 10.0f, 0.0f), glm::vec3(0.0f, 180.0f, 50.0f));
	backwardBlackHole = new Spirit("BlackHole.fbx", glm::vec3(50.0f, 250.0f, 50.0f), glm::vec3(10.0f, 10.0f, 0.0f), glm::vec3(0.0f, 180.0f, 50.0f));
	viewPlane = new Spirit("Eagle.fbx", glm::vec3(0.0f, 50.0f, 0.0f), glm::vec3(0.002f, 0.002f, 0.002f), glm::vec3(253.0f, 180.0f, 0.0f));

	sceneIndex = 0;
	isForwardShow = false;
	isBackwardShow = false;

	initDepthMapFBO();
	initScenePast();
	initSceneNow();
}

void SceneController::Draw(Shader shader, float time)
{

	if (sceneIndex != 0)
		isBackwardShow = true;
	else
		isBackwardShow = false;
	if (sceneIndex != allScenes.size() - 1)
		isForwardShow = true;
	else
		isForwardShow = false;

	if(isForwardShow)
		forwardBlackHole->Draw(shader, time);
	if(isBackwardShow)
		backwardBlackHole->Draw(shader, time);

	allScenes[sceneIndex]->Draw(shader, time);
	viewPlane->Draw(shader, time);

	if (isPressedThisFrame) {
		pressedCount++;
		fontRender->RenderCharacter(thisFramePressed, 25.0f, 25.0f, 1.0f, glm::vec3(1.0f, 1.0f, 0.5f));
		if (pressedCount > 20) {
			isPressedThisFrame = false;
			pressedCount = 0;
		}
	}
}

SceneController::SceneController()
{
	blackHoleSensitivity = 5.0f;
}

SceneController::~SceneController()
{
	delete forwardBlackHole;
	delete backwardBlackHole;
	delete viewPlane;
	for (auto & s : allScenes) {
		delete s;
	}
}

void SceneController::setThisFramePressed(const char pressed) {
	isPressedThisFrame = true;
	if (thisFramePressed == pressed) {
		pressedCount = 0;
	}
	thisFramePressed = pressed;
}

void SceneController::sceneChangeDetector()
{
	float dis;
	// 计算消耗大，先粗略判断
	if (isForwardShow && blackHoleDistancePreEstimate(forwardBlackHole->position)) {
		dis = distanceOfPositions(viewPlane->position, forwardBlackHole->position);
		//printf("forwardHole dis: %f\n\n", dis);
		if (dis < blackHoleSensitivity) {
			sceneIndex++;
		}
	}
	if (isBackwardShow && blackHoleDistancePreEstimate(backwardBlackHole->position)) {
		dis = distanceOfPositions(viewPlane->position, backwardBlackHole->position);
		//printf("backwardHole dis: %f\n\n", dis);
		if (dis < blackHoleSensitivity) {
			sceneIndex--;
		}
	}
	//printf("hole pos: %f %f %f\n", forwardBlackHole->position.x, forwardBlackHole->position.y, forwardBlackHole->position.z);
	//printf("plane pos: %f %f %f\n", viewPlane->position.x, viewPlane->position.y, viewPlane->position.z);
	//dis = distanceOfPositions(viewPlane->position, forwardBlackHole->position);
	//printf("dis: %f\n\n", dis);
}
inline void SceneController::initScenePast()
{
	allScenes.push_back(new Scene());
	allScenes.back()->addCharacter("past_static.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-90.0f, 0.0f, 0.0f));
}
inline void SceneController::initSceneNow()
{
	allScenes.push_back(new Scene());
	allScenes.back()->addCharacter("nowSence/now_walking_people.fbx", glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.005f, 0.005f, 0.005f), glm::vec3(0.0f, 0.0f, 0.0f));
	allScenes.back()->addCharacter("nowSence/now_stay_people.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f));
	allScenes.back()->addCharacter("nowSence/now_map_v1.fbx", glm::vec3(0.0f, 2.0f, -1.0f), glm::vec3(20.0f, 20.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	allScenes.back()->addCharacter("nowSence/now_cars_upper.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.59f, 1.59f, 1.59f), glm::vec3(90.0f, 270.0f, 180.0f));
	allScenes.back()->addCharacter("nowSence/now_cars_lower.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.096f, 2.096f, 2.096f), glm::vec3(-90.0f, 0.0f, 0.0f));
	allScenes.back()->addCharacter("nowSence/now_upper_half_v1.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	allScenes.back()->addCharacter("nowSence/now_lower_half_v1.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f), glm::vec3(0.0f, 90.0f, 0.0f));
}

bool SceneController::blackHoleDistancePreEstimate(const glm::vec3& holePos) const {
	if (fabsf(holePos.x - viewPlane->position.x) > blackHoleSensitivity ||
		fabsf(holePos.y - viewPlane->position.y) > blackHoleSensitivity ||
		fabsf(holePos.z - viewPlane->position.z > blackHoleSensitivity)) {
		return false;
	}
	return true;
}

void SceneController::initDepthMapFBO() {
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#endif // !SCENE_CONTROLLER__H
