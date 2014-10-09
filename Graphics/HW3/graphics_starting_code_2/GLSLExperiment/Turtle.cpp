#include "Turtle.h"


Turtle::Turtle(void)
{
}

void Turtle::push(){
	rotStack.push_back(rot);
	locStack.push_back(loc);
}

void Turtle::pop(){
	rot = rotStack.back();
	loc = locStack.back();
	rotStack.pop_back();
	locStack.pop_back();
}

void Turtle::forward(){
	loc += rot * vec4(0,0,1, 1);
}

Turtle::~Turtle(void)
{
}
