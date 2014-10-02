#pragma once

#include "Angel.h"
#include <string>
#include <map>

using namespace std;

class Grammer
{
public:
	float len;
	vec3 rot;
	string pattern;
	Grammer(char* fileName);
	~Grammer(void);
};

