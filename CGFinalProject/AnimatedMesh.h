
#ifndef ANIMATED_MESH_H
#define ANIMATED_MESH_H
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <learnopengl/shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "ogldev_util.h"
#include "math_3d.h"

#define BONE_INFO_NUM 4

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	//// texCoords
	//glm::vec2 TexCoords;
	//// tangent
	//glm::vec3 Tangent;
	//// bitangent
	//glm::vec3 Bitangent;
	glm::ivec4 boneID; //影响骨骼ID

	glm::vec4 boneWeight; // 对应权重

	Vertex() {
		Position = glm::vec3(0.0f, 0.0f, 0.0f);
		Normal = glm::vec3(0.0f, 0.0f, 0.0f);
		boneID = glm::ivec4(-1, 0, 0, 0);
		boneWeight = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	void AddBoneData(uint BoneID, float Weight) {
		for (unsigned int i = 0; i < BONE_INFO_NUM; i++) {
			if (Weight > boneWeight[i]) {
				for (unsigned int j = BONE_INFO_NUM - 1; j > i; j--) {
					boneWeight[j] = boneWeight[j - 1];
					boneID[j] = boneID[j - 1];
				}
				boneWeight[i] = Weight;
				boneID[i] = BoneID;
				break;
			}
		}
	}

	void normalizeBoneWeight() {
		assert(0);
	}


};

struct Material {
	//材质颜色光照
	glm::vec4 Ka;
	//漫反射
	glm::vec4 Kd;
	//镜反射
	glm::vec4 Ks;
};


class AnimatedMesh
{
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	unsigned int VAO;
	Material mats;

	AnimatedMesh(vector<Vertex> vertices, vector<unsigned int> indices, Material mats) {
		this->vertices = vertices;
		this->indices = indices;
		this->mats = mats;

		// now that we have all the required data, set the vertex buffers and its attribute pointers.
		setupMesh();
	}
	~AnimatedMesh() {

	}

	void Draw(Shader shader)
	{
		shader.setVec3("material.ambient", mats.Ka.x, mats.Ka.y, mats.Ka.z);
		shader.setVec3("material.diffuse", mats.Kd.x, mats.Kd.y, mats.Kd.z);
		shader.setVec3("material.specular", mats.Ks.x, mats.Ks.y, mats.Ks.z); // specular lighting doesn't have full effect on this object's material


		// draw mesh
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
private:
	unsigned int VBO, EBO;
	void setupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		glEnableVertexAttribArray(2);
		glVertexAttribIPointer(2, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneID)); //使用整型

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneWeight));

		glBindVertexArray(0);
	}
};


#endif // !ANIMATED_MESH_H
