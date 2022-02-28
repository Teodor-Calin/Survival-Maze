#pragma once

#include "utils/glm_utils.h"

using namespace std;

class Projectile3D {
public:
    float distance;
    glm::vec3 direction;
    glm::vec3 position;

    Projectile3D(glm::vec3 position, glm::vec3 direction);
};