#include "CTM.h"
#include <vector>

CTM::CTM(void){
	this->model = Angel::identity();
	this->stack = new vector<mat4>();
}

void CTM::pushMatrix(void){
	this->stack->push_back(model);
	apply();
}

void CTM::popMatrix(void){
	this->model = this->stack->back();
	this->stack->pop_back();
	apply();
}

void CTM::loadIdentity(void){
	this->stack->clear();
	this->model = Angel::identity();
	apply();
}

void CTM::translate(float x, float y, float z){
	model = Angel::Translate(x, y, z) * model;	
	apply();
}

void CTM::translate(vec4 t){
	this->translate(t.x, t.y, t.z);
}

void CTM::rotateX(float theta){
	model = Angel::RotateX(theta) * model;
	apply();
}

void CTM::rotateY(float theta){
	model = Angel::RotateY(theta) * model;
	apply();
}

void CTM::rotateZ(float theta){
	model = Angel::RotateZ(theta) * model;
	apply();
}


void CTM::scale(float x, float y, float z){
	model = Angel::Scale(x, y, z) * model;
	apply();
}

void CTM::scale(float s){
	this->scale(s, s, s);
}

void CTM::apply(){
	glUniformMatrix4fv(CTM::loc, 1, FALSE, Angel::transpose(model));
}

CTM::~CTM(void){
	free(this->stack);
}