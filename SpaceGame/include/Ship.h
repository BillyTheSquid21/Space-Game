#pragma once
#ifndef SHIP_H
#define SHIP_H

#include "GameObject.h"
#include "Chunk.h"

static class ShipDeath : public GameObject
{
public:
	ShipDeath() = default;
	ShipDeath(float size);
	void render();
	void update(double deltaTime);
	void startDeath(float x, float y, float direction);

	//triangles
	Tri m_Tri1;
	Tri m_Tri2;
	Tri m_Tri3;
private:

	//distance
	float m_DeathX = 0.0f;
	float m_DeathY = 0.0f;
	float m_Angle = 0.0f;
};

class Ship : public GameObject
{
public:
	Ship() = default;
	Ship(float size);
	~Ship() = default;

	//functions
	void render();
	void update(double deltaTime);
	void translate(float deltaX, float deltaY);
	void rotate(float angle);
	void resetRotation();
	void position(float xNew, float yNew);

	//kinematics
	void accelerate(float a);
	void accelerate(float a, float angle);
	void brake(double deltaTime);
	void resetSpeed();

	//animation
	void timeOfDeath(double time);
	bool death(double deltaTime);

	//getters - travel direction adds 90 degrees for consistency
	float direction() const { return m_CumulativeAngle; }
	float travelDirection() const { return atan2(m_VelocityY, m_VelocityX); }
	ChunkLocation location() const { return m_CurrentChunk; }
	Tri* triangle() { return &m_Ship; }
	bool dying() const { return m_Dying; }

private:

	//Kinematics
	float m_CumulativeAngle = 0.0f;
	float m_VelocityX = 0.0f;
	float m_VelocityY = 10.0f;
	float m_MaxSpeed = 10000.0f;
	void accelerateVelocity(float newVelocityX, float newVelocityY);

	//current chunk
	ChunkLocation m_CurrentChunk = { 0, 0 };

	//death
	double m_TimeOfDeath = 0.0;
	double m_DeathTimer = 0.0;
	bool m_Dying = false;
	ShipDeath m_DeathAnimation;

	//color
	float r;
	float g;
	float b;
	bool rUp = true;
	bool bUp = true;
	bool gUp = false;

	void DoTheFunky(float dt);
	Tri m_Ship;
	Line m_Direction;
};

#endif