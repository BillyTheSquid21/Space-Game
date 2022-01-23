#pragma once
#ifndef SHIP_H
#define SHIP_H

#include "GameObject.h"
#include "Chunk.h"
#include "SGPhysics.h"
#include "Bullets.h"

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
	void startDeath(float x, float y, float direction, float speed); //Tells animation where to origin and what direction to progress in

	//Triangles
	Tri m_Tri1;
	Tri m_Tri2;
	Tri m_Tri3;
private:

	//Kinematic values
	float m_DeathX = 0.0f;
	float m_DeathY = 0.0f;
	float m_DeathVelocity = 0.0f;
	float m_Angle = 0.0f;
};

//Ship object
class Ship : public GameObject
{
public:
	//Constructors
	Ship() = default;
	Ship(float size, Component3f color);
	~Ship();

	//Functions
	void render();
	void update(double deltaTime);
	void translate(float deltaX, float deltaY);
	void rotate(float angle);
	void resetRotation();
	void position(float xNew, float yNew);

	//Fighting
	void shoot();	//triggers from button input
	bool hit();
	bool hasShot(); //if ship has shot, bullet is added to world

	//Kinematics
	void accelerate(float a);
	void accelerate(float a, float angle);
	void brake(double deltaTime);
	void setMaxSpeed(float max) { m_MaxSpeed = max; }
	void resetSpeed();

	//Animation
	void timeOfDeath(double time);
	bool death(double deltaTime);
	bool death(); //version for not updating animation

	//Getters - travel direction adds 90 degrees for consistency
	float direction() const { return m_CumulativeAngle; }
	float travelDirection() const { return CalculateAngle(0.0f, 0.0f, m_VelocityX, m_VelocityY); }
	float veloctiy() const { return sqrt((m_VelocityX * m_VelocityX) + (m_VelocityY * m_VelocityY)); }
	Component2f velocityComponent() const { return {m_VelocityX, m_VelocityY}; }
	Component2f gravityComponent() const { return m_CurrentGravityResultant; }

	//Returns whether the ship is dying
	bool dying() const { return m_Dying; }

	//Returns chunk ship is in
	ChunkLocation location() const { return m_CurrentChunk; }

	//Returns shape underlaying ship
	Tri* triangle() { return &m_Ship; }

private:

	//Kinematics
	float m_CumulativeAngle = 0.0f;
	float m_VelocityX = 0.0f;
	float m_VelocityY = 10.0f;
	float m_MaxSpeed = 10000.0f;
	Component2f m_CurrentGravityResultant = { 0.0f, 0.0f };			//stores the resultant acceleration on ship externally per frame
	void accelerateVelocity(float newVelocityX, float newVelocityY);
	
	//Current chunk
	ChunkLocation m_CurrentChunk = { 0, 0 };

	//Size
	float m_Size = 0.0f;

	//Death
	unsigned char hitCount = 0;
	double m_TimeOfDeath = 0.0;
	double m_DeathTimer = 0.0;
	bool m_Dying = false;
	ShipDeath* m_DeathAnimation = nullptr;

	//Color
	float m_Red;
	float m_Green;
	float m_Blue;
	bool m_RedUp = true;
	bool m_BlueUp = true;
	bool m_GreenUp = false;

	//Bullets
	bool shot = false;
	bool ableToShoot = false;
	void canShoot(double deltaTime);
	double timeSinceShot = 0.0;
	double cooldown = 0.1;

	//Drawing
	void DoTheFunky(float dt);
	Tri m_Ship;
	Line m_Direction;
};

#endif