#pragma once
#ifndef SG_PHYSICS_H
#define SG_PHYSICS_H

#include "SGUtil.h"
#include "ShapeFactory.h"

//Constants
const float SG_PHYS_G = 3256.0f; //gravity constant

//Defines structure containing magnitude and direction of a force
struct Force
{
	float magnitude;
	float direction;
};

//Calculations
float CalculateDistance(float x1, float y1, float x2, float y2);
float CalculateAngle(float x1, float y1, float x2, float y2);
Force CalculateGravity(float x1, float y1, float x2, float y2, float mass);

//Collision - polygon circle for if a small object is inside, and is quick and dirty
bool PolygonCircleCollision(void* polygon, unsigned int verticeCount, float xCircle, float yCircle, float radius);
bool CircleCollision(float x, float y, unsigned int hitCircleRadius, float xCircle, float yCircle, float radius);
#endif