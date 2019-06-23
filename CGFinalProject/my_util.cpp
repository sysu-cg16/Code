#include "ogldev_util.h"
#include <cmath>

float distanceOfPositions(const glm::vec3 pos1, const glm::vec3 pos2) {
	glm::vec3 pos = pos1 - pos2;
	return (float)sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
}

bool isFloatEqual(float a, float b) {
	return fabs(a - b) < 0.0000001f;
}

unsigned int SCR_WIDTH = 1024;
unsigned int SCR_HEIGHT = 1024;
const unsigned int SHADOW_WIDTH = 10240, SHADOW_HEIGHT = 10240;