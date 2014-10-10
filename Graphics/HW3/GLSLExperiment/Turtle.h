#pragma once

#include "Angel.h"
#include <vector>

using namespace std;

class Turtle
{
public:
	mat4 rot;
	vec4 loc;
	vector<mat4> rotStack;
	vector<vec4> locStack;
	Turtle(void);
	void push();
	void pop();
	void forward();
	~Turtle(void);
};

