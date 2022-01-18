#pragma once
#ifndef ORBITALS_H
#define ORBITALS_H

#include "GameObject.h"
#include "Chunk.h"

//Planet or star sort of objects
class Orbital : public GameObject 
{
public:

	//Constructors
	Orbital() = default;
	~Orbital() = default;

	//Inherited
	void render();

	//Getters
	ChunkLocation location() const { return m_CurrentChunk; }
	float mass() const { return m_Mass; }

	//Pointer
	ObjectPointer pointer = NULL_OBJECT;

	//Circle for collision
	Circle circle() const { return m_Orbital; }

	float radius() const { return m_Radius; }

protected:

	//Radius and Mass
	float m_Radius = 0;
	float m_Mass = 0;

	//Current chunk
	ChunkLocation m_CurrentChunk = { 0, 0 };
	
	//Object vertices
	Circle m_Orbital;
};

//Star
enum class StarType {
	RED_DWARF, SOLAR, RED_GIANT, BLUE_GIANT			//Defines type of star
};

//Struct to provide info to rebuild star
struct StarInfo {
	float x; float y; float mass; StarType temp;	//Contains the minimum information to reconstruct a star
};

//Star probabilities
const int STAR_MAX_PROBABILITY = 1000;
struct StarProbability
{
	//All have a range within max - randomiser will land at a number less than a probability
	unsigned int RED_DWARF = 400;
	unsigned int SOLAR = 650;
	unsigned int RED_GIANT = 850;
	unsigned int BLUE_GIANT = STAR_MAX_PROBABILITY;
};

//Stores probabilities
static StarProbability s_StarProbabilities;

//Star randomisation functions
StarType ReturnRandomStar();
float GetRandomStarMass(StarType type);
float GetRandomOrbitDistance(StarType type);

//Star object
class Star : public Orbital
{
public:
	//Constructors
	using Orbital::Orbital;
	Star(StarInfo star, unsigned int index);

	//Inherited
	void render();
	void update(double deltaTime);

	//Unload - returns instructions to rebuild
	StarInfo unload() const { return {m_XPos, m_YPos, m_Mass, m_Type}; }
	StarType type() const { return m_Type; }

private:
	//Initial colors for pulse animation
	float m_InitialRed = 0.0f;
	float m_InitialBlue = 0.0f;

	//Current colors
	float m_Red = 0.0f;
	float m_Green = 0.0f;
	float m_Blue = 0.0f;
	//Controls whether color is increasing in value
	bool redUp = true;
	bool blueUp = false;

	StarType m_Type;
	Circle m_Light;
};

//Defines types of planets
enum class PlanetType
{
	BLUE_ROCKY, RED_ROCKY, GREEN_ROCKY, GREY_ROCKY,
	BLUE_GAS, RED_GAS, GREEN_GAS, YELLOW_GAS,

	//Default
	END
};

//Defines minimum information to rebuild planet
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

//Stores probabilities
static PlanetProbability s_PlanetProbabilities;

//Gives random planet type
PlanetType ReturnRandomPlanet(); 

//Colors planet based on type
void ColorPlanet(PlanetType type, StarType parentColor, void* orbital, float orbitDistance);

//Planet object
class Planet : public Orbital
{
public:
	//Constructors
	using Orbital::Orbital;
	Planet(PlanetInfo planet, StarType parentColor, unsigned int index);

	//Functions - unload returns instructions
	void update(double deltaTime);
	PlanetInfo unload() const { return {m_Mass, m_Angle, m_Velocity, m_Type, m_RotationX, m_RotationY, m_OrbitDistance}; }

private:

	//Kinematic values
	float m_RotationX;
	float m_RotationY;
	float m_Angle;
	float m_Velocity; 
	float m_OrbitDistance;

	PlanetType m_Type;
};

#endif
