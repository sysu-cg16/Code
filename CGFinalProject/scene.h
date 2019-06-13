#ifndef SCENE__H
#define SCENE__H

#include "character.h"
#include <vector>

class Scene
{
public:
	~Scene();
	void Draw(Shader shader, float time);
	void addCharacter(std::string Path, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 angles = glm::vec3(0.0f, 0.0f, 0.0f));
private:
	vector<Character*> allCharacters;
};

void Scene::Draw(Shader shader, float time)
{
	for (auto & ch : allCharacters) {
		ch->Draw(shader, time);
	}
}

void Scene::addCharacter(std::string Path, glm::vec3 position, glm::vec3 scale, glm::vec3 angles)
{
	allCharacters.push_back(new Character(Path, position, scale, angles));
}

Scene::~Scene() {
	for (auto &ch : allCharacters) {
		delete ch;
	}
}


#endif // !SCENE__H
