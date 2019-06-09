#ifndef ANIMATED_MODEL_H
#define ANIMATED_MODEL_H
#include "AnimatedMesh.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include <map>

struct Bone {
	std::string name;
	//unsigned int coMeshID;
	Matrix4f boneOffset;
	Matrix4f FinalTransformation;
};


class AnimatedModel
{
public:
	vector<AnimatedMesh> meshes;
	string directory;
	const aiScene* pScene;
	Matrix4f globalInverseTransform;
	vector<Bone> allBones;
	std::map<string, unsigned int> boneMap;
	unsigned int numBones;

	void Draw(Shader shader, float time) {
		if (pScene->HasAnimations()) {
			vector<Matrix4f> Transforms;
			BoneTransform(time, Transforms);
			char uniformName[50];
			for (unsigned int i = 0; i < numBones; i++) {
				sprintf(uniformName, "gBones[%d]", i);
				GLuint location = glGetUniformLocation(shader.ID, uniformName);
				glUniformMatrix4fv(location, 1, GL_TRUE, (const GLfloat*)Transforms[i]);
			}
		}
		for (auto& mesh : meshes)
		{
			mesh.Draw(shader);
		}
	}

	AnimatedModel(string const &path) {
		loadModel(path);
	}
	//~AnimatedModel();

private:
	Assimp::Importer importer;

	void loadModel(string const &path)
	{
		// read file via ASSIMP
		pScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
		// check for errors
		if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		globalInverseTransform = pScene->mRootNode->mTransformation;
		globalInverseTransform.Inverse();


		// process ASSIMP's root node recursively
		processNode(pScene->mRootNode, pScene);
	}

	void processNode(aiNode *node, const aiScene *scene)
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(i, mesh, scene));
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}

	AnimatedMesh processMesh(unsigned int meshID, aiMesh *mesh, const aiScene *scene)
	{
		// data to fill
		vector<Vertex> vertices;
		vector<unsigned int> indices;

		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			// positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// texture coordinates
			//if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			//{
			//	glm::vec2 vec;
			//	// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			//	// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			//	vec.x = mesh->mTextureCoords[0][i].x;
			//	vec.y = mesh->mTextureCoords[0][i].y;
			//	vertex.TexCoords = vec;
			//}
			//else
			//	vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			//// tangent
			//if (mesh->mTangents) {
			//	vector.x = mesh->mTangents[i].x;
			//	vector.y = mesh->mTangents[i].y;
			//	vector.z = mesh->mTangents[i].z;
			//	vertex.Tangent = vector;
			//}
			//else
			//	vertex.Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
			//// bitangent
			//if (mesh->mBitangents) {
			//	vector.x = mesh->mBitangents[i].x;
			//	vector.y = mesh->mBitangents[i].y;
			//	vector.z = mesh->mBitangents[i].z;
			//	vertex.Bitangent = vector;
			//}
			//else
			//	vertex.Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
			vertices.push_back(vertex);
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		Material mat;
		aiColor3D color;

		//读取mtl文件顶点数据

		material->Get(AI_MATKEY_COLOR_AMBIENT, color);
		mat.Ka = glm::vec4(color.r, color.g, color.b, 1.0);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		mat.Kd = glm::vec4(color.r, color.g, color.b, 1.0);
		if (mat.Ka.x == 0 && mat.Ka.y == 0 && mat.Ka.z == 0) {
			mat.Ka = mat.Kd;
		}
		material->Get(AI_MATKEY_COLOR_SPECULAR, color);
		mat.Ks = glm::vec4(color.r, color.g, color.b, 1.0);

		// 加载骨骼权重信息到顶点，并将骨骼加入allBones和boneMap
		for (uint i = 0; i < mesh->mNumBones; i++) {
			unsigned int BoneIndex = 0;
			string BoneName(mesh->mBones[i]->mName.data);
			if (boneMap.find(BoneName) == boneMap.end()) {
				// Allocate an index for a new bone
				BoneIndex = numBones;
				numBones++;
				Bone tmpBone;
				tmpBone.boneOffset = mesh->mBones[i]->mOffsetMatrix;
				tmpBone.name = BoneName;
				// 加入map
				boneMap[BoneName] = BoneIndex;
				// 加入allBones
				allBones.push_back(tmpBone);
			}
			else {
				BoneIndex = boneMap[BoneName];
			}
			for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
				unsigned int vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
				float weight = mesh->mBones[i]->mWeights[j].mWeight;
				// 给vertices添加影响骨骼信息
				vertices[vertexID].AddBoneData(BoneIndex, weight);
			}
		}
		return AnimatedMesh(vertices, indices, mat);
	}


	void BoneTransform(float TimeInSeconds, vector<Matrix4f>& Transforms)
	{
		Matrix4f Identity;
		Identity.InitIdentity();

		float TicksPerSecond = (float)(pScene->mAnimations[0]->mTicksPerSecond != 0 ? pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
		float TimeInTicks = TimeInSeconds * TicksPerSecond;
		float AnimationTime = fmod(TimeInTicks, (float)pScene->mAnimations[0]->mDuration);

		ReadNodeHeirarchy(AnimationTime, pScene->mRootNode, Identity);

		Transforms.resize(numBones);

		for (uint i = 0; i < numBones; i++) {
			Transforms[i] = allBones[i].FinalTransformation;
		}
	}

	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform)
	{
		string NodeName(pNode->mName.data);

		const aiAnimation* pAnimation = pScene->mAnimations[0]; //选择动画

		Matrix4f NodeTransformation(pNode->mTransformation);

		const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

		if (pNodeAnim) {
			// Interpolate scaling and generate scaling transformation matrix
			aiVector3D Scaling;
			CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
			Matrix4f ScalingM;
			ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

			// Interpolate rotation and generate rotation transformation matrix
			aiQuaternion RotationQ;
			CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
			Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());

			// Interpolate translation and generate translation transformation matrix
			aiVector3D Translation;
			CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
			Matrix4f TranslationM;
			TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

			// Combine the above transformations
			NodeTransformation = TranslationM * RotationM * ScalingM;
		}

		Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

		if (boneMap.find(NodeName) != boneMap.end()) {
			uint BoneIndex = boneMap[NodeName];
			allBones[BoneIndex].FinalTransformation = globalInverseTransform * GlobalTransformation * allBones[BoneIndex].boneOffset;
		}

		for (uint i = 0; i < pNode->mNumChildren; i++) {
			ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
		}
	}


	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		if (pNodeAnim->mNumPositionKeys == 1) {
			Out = pNodeAnim->mPositionKeys[0].mValue;
			return;
		}

		uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
		uint NextPositionIndex = (PositionIndex + 1);
		assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
		float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}


	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumRotationKeys == 1) {
			Out = pNodeAnim->mRotationKeys[0].mValue;
			return;
		}

		uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
		uint NextRotationIndex = (RotationIndex + 1);
		assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
		float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
		aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
		Out = Out.Normalize();
	}


	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		if (pNodeAnim->mNumScalingKeys == 1) {
			Out = pNodeAnim->mScalingKeys[0].mValue;
			return;
		}

		uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
		uint NextScalingIndex = (ScalingIndex + 1);
		assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
		float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
		float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
		assert(Factor >= 0.0f && Factor <= 1.0f);
		const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
		const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
		aiVector3D Delta = End - Start;
		Out = Start + Factor * Delta;
	}

	uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
				return i;
			}
		}

		assert(0);

		return 0;
	}


	uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumRotationKeys > 0);

		for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
				return i;
			}
		}

		assert(0);

		return 0;
	}


	uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		assert(pNodeAnim->mNumScalingKeys > 0);

		for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
			if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
				return i;
			}
		}

		assert(0);

		return 0;
	}

	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
	{
		for (uint i = 0; i < pAnimation->mNumChannels; i++) {
			const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

			if (string(pNodeAnim->mNodeName.data) == NodeName) {
				return pNodeAnim;
			}
		}

		return NULL;
	}
};





#endif // !ANIMATED_MODEL_H

