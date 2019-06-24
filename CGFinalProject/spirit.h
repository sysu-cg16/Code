#ifndef SPIRIT_H
#define SPIRIT_H
#include<string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <learnopengl/model.h>

#include "AnimatedModel.h"

class Spirit
{
public:
	Spirit(std::string Path, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 angles = glm::vec3(0.0f, 0.0f, 0.0f))
	: spiritModel(("resources/" + Path).data()) {
		this->position = position;
		this->angles = angles;
		this->scale = scale;
	}
	void Draw(Shader shader, float time) {
		shader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::rotate(model, glm::radians(angles.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(angles.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(angles.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, scale);
		shader.setMat4("model", model);

		spiritModel.Draw(shader, time);
	}

	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 angles;
	glm::vec3 angles2;
private:

	AnimatedModel spiritModel;
};

#endif // !SPIRIT_H



