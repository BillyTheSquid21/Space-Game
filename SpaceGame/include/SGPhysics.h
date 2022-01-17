#pragma once
#ifndef SG_PHYSICS_H
#define SG_PHYSICS_H

#include "SGUtil.h"

//Constants
const float SG_PHYS_G = 3256.0f; //gravity constant

struct Force
{
	float magnitude;
	float direction;
};

//Calculations
float CalculateDistance(float x1, float y1, float x2, float y2);
Force CalculateGravity(float x1, float y1, float x2, float y2, float mass);

#endif