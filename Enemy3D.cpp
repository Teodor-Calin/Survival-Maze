#include "Enemy3D.h"

using namespace std;

Enemy3D::Enemy3D(int movement_type, glm::vec3 center_position) {
	
	this->movement_type = movement_type;
	this->center_position = center_position;
	this->angle = 0;
	this->speed = ((float)(rand() % 100 + 100)) / 100;
}