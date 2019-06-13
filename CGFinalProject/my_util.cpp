#include "ogldev_util.h"

float distanceOfPositions(const glm::vec3 pos1, const glm::vec3 pos2) {
	glm::vec3 pos = pos1 - pos2;
	return (float)sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
}