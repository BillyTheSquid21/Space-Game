#pragma once
#ifndef SHIP_H
#define SHIP_H

#include "GameObject.h"
#include "Chunk.h"

//Object that plays a short animation on ship death
static class ShipDeath : public GameObject
{
public:

	//Constructors
	ShipDeath() = default;
	ShipDeath(float size);

	//Functions
	void render();
	void update(double deltaTime);
	void startDeath(float x, float y, float direction); //Tells animation where to origin and what direction to progress in

	//Triangles
	Tri m_Tri1;
	Tri m_Tri2;
	Tri m_Tri3;
private:

	//Kinematic values
	float m_DeathX = 0.0f;
	float m_DeathY = 0.0f;
	float m_Angle = 0.0f;
};

//Ship object
class Ship : public GameObject
{
public:
	//Constructors
	Ship() = default;
	Ship(float size);
	~Ship() = default;

	//Functions
	void render();
	void update(double deltaTime);
	void translate(float deltaX, float deltaY);
	void rotate(float angle);
	void resetRotation();
	void position(float xNew, float yNew);

	//Kinematics
	void accelerate(float a);
	void accelerate(float a, float angle);
	void brake(double deltaTime);
	void resetSpeed();

	//Animation
	void timeOfDeath(double time);
	bool death(double deltaTime);

	//Getters - travel direction adds 90 degrees for consistency
	float direction() const { return m_CumulativeAngle; }
	float travelDirection() const { return atan2(m_VelocityY, m_VelocityX); }
	bool dying() const { return m_Dying; }
	ChunkLocation location() const { return m_CurrentChunk; }
	Tri* triangle() { return &m_Ship; }

private:

	//Kinematics
	float m_CumulativeAngle = 0.0f;
	float m_VelocityX = 0.0f;
	float m_VelocityY = 10.0f;
	float m_MaxSpeed = 10000.0f;
	void accelerateVelocity(float newVelocityX, float newVelocityY);

	//Current chunk
	ChunkLocation m_CurrentChunk = { 0, 0 };

	//Death
	double m_TimeOfDeath = 0.0;
	double m_DeathTimer = 0.0;
	bool m_Dying = false;
	ShipDeath m_DeathAnimation;

	//Color
	float r;
	float g;
	float b;
	bool rUp = true;
	bool bUp = true;
	bool gUp = false;

	//Drawing
	void DoTheFunky(float dt);
	Tri m_Ship;
	Line m_Direction;
};

#endif