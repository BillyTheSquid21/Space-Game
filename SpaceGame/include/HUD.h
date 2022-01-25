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
	const float m_XPos = -810.0f; const float m_YPos = -120.0f;
	float m_CumulativeRotationFacing = 0.0f;
	float m_CumulativeRotationMoving = 0.0f;
};

#endif