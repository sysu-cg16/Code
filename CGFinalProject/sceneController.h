#ifndef SCENE_CONTROLLER__H
#define SCENE_CONTROLLER__H

#include "scene.h"
#include <vector>

class SceneController
{
public:
	SceneController();
	~SceneController();
	void Draw(Shader shader, float time);
	void init();
	float blackHoleSensitivity;
	Character* forwardBlackHole;
	Character* backwardBlackHole;
	Character* viewPlane;
	void sceneChangeDetector();
private:
	void initScenePast();
	void initSceneNow();
	vector<Scene*> allScenes;
	int sceneIndex;
	bool isForwardShow;
	bool isBackwardShow;
	bool blackHoleDistancePreEstimate(const glm::vec3& holePos) const;
};

inline void SceneController::init()
{
	forwardBlackHole = new Character("BlackHole.fbx", glm::vec3(-300.0f,220.0f, 450.0f), glm::vec3(5.0f, 5.0f, 0.0f), glm::vec3(0.0f, 180.0f, 50.0f));
	backwardBlackHole = new Character("BlackHole.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f, 5.0f, 0.0f), glm::vec3(0.0f, 180.0f, 50.0f));
	viewPlane = new Character("Spaceship3.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(285.0f, 0.0f, 182.0f));
	sceneIndex = 0;
	isForwardShow = false;
	isBackwardShow = false;

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

	//// 调试用
	//forwardBlackHole->Draw(shader, time);

	allScenes[sceneIndex]->Draw(shader, time);
	viewPlane->Draw(shader, time);
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
	allScenes.back()->addCharacter("past_1000.fbx");
}
inline void SceneController::initSceneNow()
{
	allScenes.push_back(new Scene());
	allScenes.back()->addCharacter("now_map.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(20.0f, 20.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	allScenes.back()->addCharacter("now_upper_half_v1.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(8.0f, 8.0f, 8.0f), glm::vec3(-90.0f, 0.0f, 0.0f));
	allScenes.back()->addCharacter("now_lower_half_v1.fbx", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f), glm::vec3(0.0f, 180.0f, 0.0f));
}

bool SceneController::blackHoleDistancePreEstimate(const glm::vec3& holePos) const {
	if (fabsf(holePos.x - viewPlane->position.x) > blackHoleSensitivity ||
		fabsf(holePos.y - viewPlane->position.y) > blackHoleSensitivity ||
		fabsf(holePos.z - viewPlane->position.z > blackHoleSensitivity)) {
		return false;
	}
	return true;
}
#endif // !SCENE_CONTROLLER__H
