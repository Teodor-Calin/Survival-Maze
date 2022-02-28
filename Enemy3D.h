#pragma once

#include "utils/glm_utils.h"

using namespace std;

class Enemy3D {
public:
    int movement_type;
    glm::vec3 center_position;
    float angle;
    float speed;
    glm::vec3 actual_pos;

    Enemy3D(int movement_type, glm::vec3 center_position);
};