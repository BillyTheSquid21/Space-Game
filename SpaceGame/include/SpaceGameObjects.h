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
	void brake(double deltaTime);

	//getters - travel direction adds 90 degrees for consistency
	float direction() const { return m_CumulativeAngle; }
	float travelDirection() const { return atan(m_VelocityX / m_VelocityY); }
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
enum class StarType {
	RED_DWARF, SOLAR, RED_GIANT, BLUE_GIANT
};

//Struct to provide info to rebuild star
struct StarInfo {
	float x; float y; float mass; StarType temp;
};

//Star probabilities
const int STAR_MAX_PROBABILITY = 1000;
struct StarProbability
{
	//all have a range within max - if random num lands between
	unsigned int RED_DWARF = 400;
	unsigned int SOLAR = 650;
	unsigned int RED_GIANT = 850;
	unsigned int BLUE_GIANT = STAR_MAX_PROBABILITY;
};

//stores probabilities
static StarProbability s_StarProbabilities;

//Star randomisation functions to add some spice
StarType ReturnRandomStar();
float GetRandomStarMass(StarType type);
float GetRandomOrbitDistance(StarType type);

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
	StarType type() const { return m_Temperature; }

private:
	//initial red
	float m_InitialRed = 0.0f;
	float m_InitialBlue = 0.0f;

	float m_Red = 0.0f;
	float m_Green = 0.0f;
	float m_Blue = 0.0f;
	bool redUp = true;
	bool blueUp = false;

	StarType m_Temperature;
	Circle m_Light;
};

//PLANETS
enum class PlanetType
{
	BLUE_ROCKY, RED_ROCKY, GREEN_ROCKY, GREY_ROCKY,
	BLUE_GAS, RED_GAS, GREEN_GAS, YELLOW_GAS,

	//at end, and also default
	END
};

struct PlanetInfo 
{
	float mass; float angle; float velocity; PlanetType type;
	float rotationX; float rotationY; float orbitDistance;
};

//Planet probabilities
const int PLANET_MAX_PROBABILITY = 2000; 
struct PlanetProbability
{	
	//all have a range within max - if random num lands between
	unsigned int BLUE_ROCKY = 250;
	unsigned int RED_ROCKY = 600;
	unsigned int GREEN_ROCKY = 700;
	unsigned int GREY_ROCKY = 1000;
	unsigned int BLUE_GAS = 1300;
	unsigned int RED_GAS = 1600;
	unsigned int GREEN_GAS = 1900;
	unsigned int YELLOW_GAS = PLANET_MAX_PROBABILITY;
};

//stores probabilities
static PlanetProbability s_PlanetProbabilities;

//gives random planet type
PlanetType ReturnRandomPlanet(); 

//colors planet - contains data for how to
void ColorPlanet(PlanetType type, StarType parentColor, void* orbital, float orbitDistance);

class Planet : public Orbital
{
public:
	using Orbital::Orbital;
	Planet(PlanetInfo planet, StarType parentColor, unsigned int index);

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
