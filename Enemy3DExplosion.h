#pragma once

#include "utils/glm_utils.h"

using namespace std;

class Enemy3DExplosion {
public:
    glm::vec3 position;
    float time;

    Enemy3DExplosion(glm::vec3 position);
};
