#include <iostream>
#include "Wall3D.h"

using namespace std;

Wall3D::Wall3D(float x1, float y1, float x2, float y2) {
	this->x1 = x1;
	this->x2 = x2;
	this->y1 = y1;
	this->y2 = y2;
}

Wall3D::Wall3D() {
	this->x1 = NULL;
	this->x2 = NULL;
	this->y1 = NULL;
	this->y2 = NULL;
}