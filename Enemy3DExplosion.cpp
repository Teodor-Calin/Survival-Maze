#include "Enemy3DExplosion.h"

using namespace std;

Enemy3DExplosion::Enemy3DExplosion(glm::vec3 position) {

	this->position = position;
	this->time = 1.5f;
}