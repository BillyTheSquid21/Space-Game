#pragma once
#ifndef HUD_H
#define HUD_H

#include "ShapeFactory.h"
#include "Renderer.h"

class HUD
{	
public:
	HUD();
	~HUD() = default;

	void render();
	void rotateDirectionFacing(float deltaAngle);
	void rotateDirectionMoving(float deltaAngle);
	void resetFacing();
	void resetMoving();

private:
	Ring compass;
	Line directionFacing;
	Line directionMoving;
	float m_XPos; float m_YPos;
	float m_CumulativeRotationFacing = 0.0f;
	float m_CumulativeRotationMoving = 0.0f;
};

#endif