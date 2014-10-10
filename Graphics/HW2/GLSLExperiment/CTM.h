#pragma once
#include "Angel.h"
#include <vector>

using namespace std;

class CTM {
public:
	mat4 model;
	vector<mat4>* stack;
	CTM(void);
	~CTM(void);
	void pushMatrix();
	void popMatrix();
	void loadIdentity();
	void translate(float x, float y, float z);
	void translate(vec4 t);
	void rotateX(float theta);
	void rotateY(float theta);
	void rotateZ(float theta);
	void scale(float x, float y, float z);
	void scale(float s);
	void shearX(float h);
};