#include "AI.h"

AI::AI(Ship* target, Ship* assignedShip) 
	: m_Target{ target }, m_ShipControlling{ assignedShip } {

}

void AI::setWorld(World* world) {
	m_World = world;
}

void AI::update(double deltaTime) {
	//Find direction to point
	float direction = CalculateAngle(m_ShipControlling->xPos(), m_ShipControlling->yPos(),
		m_Target->xPos(), m_Target->yPos());

	//Travel direction
	float travelDirection = m_ShipControlling->travelDirection();

	float angle = 0.0f; //angle ship will point

	//Reset rotation and point at target
	if (!m_Orbit) {
		angle = direction;
		m_ShipControlling->resetRotation();
		m_ShipControlling->rotate(angle - SG_PI / 2);

		//Ship brakes if cornering and not in orbit
		if (abs(angle - travelDirection) > SG_PI / 2 && abs(angle - travelDirection) < (2 * SG_PI) - SG_PI / 2) {
			m_Brake = true;
		}
	}
	else {
		angle = travelDirection;
		m_ShipControlling->resetRotation();
		m_ShipControlling->rotate(angle - SG_PI / 2);
	}

	//Accelerate in direction
	if (m_Brake) {
		
		if (m_ShipControlling->veloctiy() < 1000.0f) {
			m_Brake = false;

			//allows accelerating here so ships don't get stuck
			m_ShipControlling->accelerate(5000.0f * deltaTime);
		}
		else {
			m_ShipControlling->brake(deltaTime);
		}
	}
	else {
		m_ShipControlling->accelerate(5000.0f * deltaTime);
	}
	
	//Here governs whether ship turns into orbit if gravity is strong - avoids crashes
	float gravX = m_ShipControlling->gravityComponent().a;
	float gravY = m_ShipControlling->gravityComponent().b;

	if (sqrt((gravX * gravX) + (gravY * gravY)) > 50.0f) {
		m_Orbit = true;
	}
	else {
		m_Orbit = false;
	}
}