#pragma once
#ifndef SPACE_GAME_OBJECTS_H
#define SPACE_GAME_OBJECTS_H

#include "GameObject.h"
#include "Chunk.h"

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

	//location
	ChunkLocation location() const { return m_CurrentChunk; }

private:

	//Kinematics
	float m_CumulativeAngle = 0.0f;
	float m_VelocityX = 0.0f;
	float m_VelocityY = 10.0f;
	float m_MaxSpeed = 8000.0f;

	//current chunk
	ChunkLocation m_CurrentChunk = { 0, 0 };

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

//Planet or star sort of objects
class Orbital : public GameObject 
{
public:

	//constructors
	Orbital() = default;
	~Orbital() = default;

	//inherited
	void render();

	//getters
	ChunkLocation location() const { return m_CurrentChunk; }
	float mass() const { return m_Mass; }

	//Pointer
	ObjectPointer pointer = NULL_OBJECT;

protected:
	//Radius and Mass
	float m_Radius = 0;
	float m_Mass = 0;

	//current chunk
	ChunkLocation m_CurrentChunk = { 0, 0 };
	
	//object vertices
	Circle m_Orbital;
};

//Star
enum class StarColor {
	RED_DWARF, SOLAR, RED_GIANT, BLUE_GIANT
};

//Struct to provide info to rebuild star
struct StarInfo {
	float x; float y; float mass; StarColor temp;
};

class Star : public Orbital
{
public:
	//Constructors
	using Orbital::Orbital;
	Star(StarInfo star, unsigned int index);

	//inherited
	void render();
	void update(double deltaTime);

	//unload - returns info to update
	StarInfo unload() const { return {m_XPos, m_YPos, m_Mass, m_Temperature}; }
	StarColor m_Temperature;

private:
	//initial red
	float m_InitialRed = 0.0f;
	float m_InitialBlue = 0.0f;

	float m_Red = 0.0f;
	float m_Green = 0.0f;
	float m_Blue = 0.0f;
	bool redUp = true;
	bool blueUp = false;

	Circle m_Light;
};

//planets
enum class PlanetType
{
	BLUE_ROCKY,
};

struct PlanetInfo 
{
	float mass; float angle; float velocity; PlanetType type;
	float rotationX; float rotationY; float orbitDistance;
};

class Planet : public Orbital
{
public:
	using Orbital::Orbital;
	Planet(PlanetInfo planet, StarColor parentColor, unsigned int index);

	void update(double deltaTime);
	PlanetInfo unload() const { return {m_Mass, m_Angle, m_Velocity, m_Type, m_RotationX, m_RotationY, m_OrbitDistance}; }

private:

	//centre of rotation
	float m_RotationX;
	float m_RotationY;
	float m_Angle;
	float m_Velocity; 
	float m_OrbitDistance;

	PlanetType m_Type;
};

#endif
