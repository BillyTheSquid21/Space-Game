#include "HUD.h"

HUD::HUD()
	: m_XPos{ -580.0f }, m_YPos(-200.0f) 
{
	//HUD initialisation
	compass = CreateRing(-580.0f, -200.0f, 80.0f, 2.5f);
	directionFacing = CreateLine(-580.0f, -200.0f, -580.0f, -120.0f, 2.5f);
	directionMoving = CreateLine(-580.0f, -200.0f, -580.0f, -120.0f, 2.5f);

	//Color facing same color as ship
	ColorShape(&directionFacing, 0.094f, 0.584f, 0.604f, Shape::LINE);

	//Color moving orange and noticeably fainter
	ColorShape(&directionMoving, 0.702f, 0.284f, 0.0f, Shape::LINE);

	//Color compass to be lighter gray
	ColorShape(&compass, 0.8f, 0.8f, 0.8f, Shape::RING);

	//Layer shapes
	LayerShape(&directionFacing, GUI_LAYER_2, Shape::LINE);
	LayerShape(&directionMoving, GUI_LAYER_2, Shape::LINE);
	LayerShape(&compass, GUI_LAYER_3, Shape::RING);

	//Make 50% transparent
	TransparencyShape(&directionFacing, 0.5f, Shape::LINE);
	TransparencyShape(&directionMoving, 0.5f, Shape::LINE);
	TransparencyShape(&compass, 0.5f, Shape::RING);
}

void HUD::render() {
	Renderer::commitShape(&compass, Shape::RING, Shape::LINE, true);
	Renderer::commitGUIPrimitive(&directionFacing, GetElementCount(Shape::LINE), Renderer::s_Line_I, Renderer::IND_LINE);
	Renderer::commitGUIPrimitive(&directionMoving, GetElementCount(Shape::LINE), Renderer::s_Line_I, Renderer::IND_LINE);
}

void HUD::rotateDirectionFacing(float deltaAngle) 
{
	RotateShape(&directionFacing, m_XPos, m_YPos, deltaAngle, Shape::LINE);
	m_CumulativeRotationFacing += deltaAngle;
}

void HUD::rotateDirectionMoving(float deltaAngle)
{
	RotateShape(&directionMoving, m_XPos, m_YPos, deltaAngle, Shape::LINE);
	m_CumulativeRotationMoving = deltaAngle;
}

void HUD::resetFacing() {
	RotateShape(&directionFacing, m_XPos, m_YPos, -m_CumulativeRotationFacing, Shape::LINE);
	m_CumulativeRotationFacing = 0.0f;
}

void HUD::resetMoving() {
	RotateShape(&directionMoving, m_XPos, m_YPos, -m_CumulativeRotationMoving, Shape::LINE);
	m_CumulativeRotationMoving = 0.0f;
}