#include "ogldev_util.h"

float distanceOfPositions(const glm::vec3 pos1, const glm::vec3 pos2) {
	glm::vec3 pos = pos1 - pos2;
	return (float)sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
}

const unsigned int SCR_WIDTH = 800 * 2;
const unsigned int SCR_HEIGHT = 600 * 2;
const unsigned int SHADOW_WIDTH = 10240, SHADOW_HEIGHT = 10240;