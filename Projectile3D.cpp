#include <iostream>
#include "Projectile3D.h"

using namespace std;

Projectile3D::Projectile3D(glm::vec3 position, glm::vec3 direction) {
	this->position = position;
	this->direction = direction;
	distance = 0;
}