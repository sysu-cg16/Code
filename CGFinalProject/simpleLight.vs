#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in ivec4 BoneIDs;
layout (location = 3) in vec4 Weights;

out vec3 FragPos;
out vec3 Normal;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 100;
uniform mat4 gBones[MAX_BONES];

void main()
{
    mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform     += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform     += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform     += gBones[BoneIDs[3]] * Weights[3];

    FragPos = vec3(model * BoneTransform * vec4(aPos, 1.0));
    
	vec3 NormalT = vec3(BoneTransform * vec4(aNormal, 0.0));
	Normal = mat3(transpose(inverse(model))) * NormalT;  
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}