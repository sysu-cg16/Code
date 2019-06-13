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
private:
	void sceneChangeDetector();
	void initScenePast();
	void initSceneNow();
	vector<Scene*> allScenes;
	int sceneIndex;
	bool isForwardShow;
	bool isBackwardShow;
};

inline void SceneController::init()
{
	forwardBlackHole = new Character("BlackHole.fbx", glm::vec3(0.0f,0.0f, 100.0f), glm::vec3(5.0f, 5.0f, 0.0f), glm::vec3(80.0f, -80.0f, 100.0f));
	backwardBlackHole = new Character("BlackHole.fbx", glm::vec3(130.0f, 100.0f, 100.0f), glm::vec3(80.0f, 80.0f, 0.0f), glm::vec3(80.0f, -80.0f, 100.0f));
	viewPlane = new Character("Spaceship3.fbx", glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, -88.0f));
	sceneIndex = 0;
	isForwardShow = false;
	isBackwardShow = false;

	initScenePast();
	//initSceneNow();
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

	//if(isForwardShow)
	//	forwardBlackHole->Draw(shader, time);
	//if(isBackwardShow)
	//	backwardBlackHole->Draw(shader, time);

	// µ÷ÊÔÓÃ
	forwardBlackHole->Draw(shader, time);

	allScenes[sceneIndex]->Draw(shader, time);
	viewPlane->Draw(shader, time);
}

SceneController::SceneController()
{
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
	if (isForwardShow) {
		dis = distanceOfPositions(viewPlane->position, forwardBlackHole->position);
		// ÅÐ¶Ï
	}
	if (isBackwardShow) {
		dis = distanceOfPositions(viewPlane->position, backwardBlackHole->position);
		// ÅÐ¶Ï
	}
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
#endif // !SCENE_CONTROLLER__H
