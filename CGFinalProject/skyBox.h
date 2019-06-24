#ifndef SKY_BOX__H
#define SKY_BOX__H

#include "AnimatedModel.h"
#include <learnopengl/camera.h>
#include <learnopengl/shader.h>
#include <string>
#include "stb_image.h"
#include <iostream>
#include "ogldev_util.h"

using std::string;

class SkyBox
{
DISALLOW_COPY_AND_ASSIGN(SkyBox)
public:
	SkyBox(Camera* camera,
		const string& PosXFilename = "resources/skyBox/hourglass_rt.png",
		const string& NegXFilename = "resources/skyBox/hourglass_lf.png",
		const string& PosYFilename = "resources/skyBox/hourglass_up.png",
		const string& NegYFilename = "resources/skyBox/hourglass_dn.png",
		const string& PosZFilename = "resources/skyBox/hourglass_ft.png",
		const string& NegZFilename = "resources/skyBox/hourglass_bk.png"
	)
		:skyModel("resources/skyBox/sphere.obj"),
		skyBoxShader("skyBox.vs", "skyBox.fs"),
		camera(camera),
		texutreFileName{ PosXFilename , NegXFilename, PosYFilename, NegYFilename, PosZFilename, NegZFilename }
	{
	}
	~SkyBox() {

	}
	void init() {
		const unsigned int types[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X,
									    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
										GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
										GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
										GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
										GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
			int width, height, nrChannels;
			unsigned char *data = stbi_load(texutreFileName[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(types[i], 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}

			stbi_image_free(data);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	void Draw() {
		skyBoxShader.use();
		GLint OldCullFaceMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
		GLint OldDepthFuncMode;
		glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, camera->Position);
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera->GetViewMatrix();
		skyBoxShader.setMat4("model", model);
		skyBoxShader.setMat4("projection", projection);
		skyBoxShader.setMat4("view", view);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		skyModel.Draw(skyBoxShader, 0.0f); // 无动作，时间不重要

		glCullFace(OldCullFaceMode);
		glDepthFunc(OldDepthFuncMode);
	}
private:
	AnimatedModel skyModel;
	Camera* camera;
	Shader skyBoxShader;
	unsigned int textureID;
	const string texutreFileName[6];
};


#endif // !SKY_BOX__H
