#pragma once
#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <cstdlib>
#include "Ship.h"
#include "Orbitals.h"
#include "Chunk.h"
#include "SGPhysics.h"

//Information a physics object needs to interact
struct WorldShip
{
	//Currently only for ships
	Ship* ship;
	Planet* closestPlanet;
	float closestPlanetDistance = 1000000.0f;
	Star* closestStar;
	float closestStarDistance = 1000000.0f;
};

//World container
class World
{
public:
	World() = default;
	~World() = default;

	void init();

	//Set renderer
	void setRenderer(Renderer* ren);

	//Create first Chunks
	void initialGenerateChunks();

	//Create and load new Chunks
	void manageChunks(ChunkLocation originChunk);

	//Render world
	void render();

	//Update world - returns false on game over
	bool update(double deltaTime, double time);

	//Does game over things
	void gameOver();

	//Add ship
	void addShip(Ship* ship);
	void setShip(Ship* ship, unsigned int index);
	bool shipsInSync(unsigned int size); //check to make sure ship lists are in sync

private:
	Renderer* m_Renderer;

	//Stores array of world ships
	std::vector<WorldShip> m_WorldShips;
	ProjectileManager m_ProjectileManager;

	//Helper functions
	void resetShipClosestPhysicsBodies();
	void updatePhysicsBodies(double deltaTime, double time);
	void updatePhysicsStars(double deltaTime, double time);
	void updatePhysicsPlanets(double deltaTime, double time);
	void updatePhysicsBullets(double deltaTime, double time);
	bool playerDeath(double deltaTime);
	void aiDeath();

	//Applies gravity
	void applyGravity();

	//Loads and unloads objects
	void unloadChunkObjects(Chunk& chunk);
	void loadChunkObjects(Chunk& chunk);

	//Generators
	void generateSolarSystem(int x, int y, Chunk& chunk);
	void generatePlanet(Star* parent, Chunk& chunk, float orbitDistance);
	void generateChunkRange(ChunkLocation originChunk);

	//Stars
	std::vector<Star> m_StarsList;
	std::vector<StarInfo> m_StarsInstructionsList;
	void checkStarGen();
	unsigned int m_StarGenCount = 2; //number of cycles before a star can gen
	bool m_GenStar = true;

	//Planets
	std::vector<Planet> m_PlanetsList;
	std::vector<PlanetInfo> m_PlanetsInstructionsList;

	//CHUNK STUFF
	//Currently loaded chunks
	std::vector<Chunk> m_ChunkList;

	//Currently unloaded chunks
	std::vector<Chunk> m_UnloadedChunks;

	//List of all chunk id's to ever be created
	std::vector<ChunkLocation> m_ChunkIDList;

	//Memory set aside for chunk id's during loading to avoid constant
	//reallocation - as always same size just iterate and modifies
	std::vector<ChunkLocation> m_ChunkIDCache;

};


#endif