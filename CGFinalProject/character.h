#ifndef CHARACTER_H
#define CHARACTER_H
#include<string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <learnopengl/model.h>

#include "AnimatedModel.h"

class Character
{
public:
	Character(std::string Path) 
	: characterModel(Path.data()) {
		position = glm::vec3(1.0f, 1.0f, 1.0f);
		scale = glm::vec3(3.0f, 3.0f, 3.0f);
		angles = 0.0f;
	}
	void Draw(Shader shader, float time) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, angles, glm::vec3(1.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale);
		model = glm::translate(model, position);
		shader.setMat4("model", model);

		characterModel.Draw(shader, time);
		//characterModel.Draw(shader);
	}

	glm::vec3 position;
	glm::vec3 scale;
	GLfloat angles;
private:

	AnimatedModel characterModel;
};

#endif // !CHARACTER_H



