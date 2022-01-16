#pragma once
#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <cstdlib>
#include "SpaceGameObjects.h"
#include "Chunk.h"

class World
{
public:
	World() = default;
	~World() = default;

	void init(Ship* player);

	//set renderer
	void setRenderer(Renderer* ren);

	//Create first Chunks
	void initialGenerateChunks();

	//Create and load new Chunks
	void manageChunks(ChunkLocation originChunk);

	void render();
	void update(double deltaTime);

private:
	Renderer* m_Renderer;

	Ship* m_PlayerPointer = nullptr;

	//loads and unloads objects
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