#pragma once
#ifndef CHUNK_H
#define CHUNK_H

#include "Renderer.h"
#include "ObjectManagement.h"

//Constants for world gen:

//Converts chunk width to world coordinates and defines chunk range generated around player
const float CHUNK_TO_WORLD_FACTOR = 16384.0f;
const unsigned char DRAW_LIMIT = 5;

//Keeps track of how many chunks are generated in one cycle
constexpr unsigned int CHUNK_CYCLES_TOTAL = ((DRAW_LIMIT * 2) - 1) * ((DRAW_LIMIT * 2) - 1);

//Define minimum distance a planet can spawn to a star
const float MINIMUM_ORBIT_RED_DWARF = 2000.0f;
const float MINIMUM_ORBIT_SOLAR = 3300.0f;
const float MINIMUM_ORBIT_RED_GIANT = 3800.0f;
const float MINIMUM_ORBIT_BLUE_GIANT = 4000.0f;

//Defines what chunk an object resides in
struct ChunkLocation
{
	int x;
	int y;
};

//Find chunk id of current object
ChunkLocation LocateChunk(float xPos, float yPos);

//Chunk is a section of the world which can contain Game Objects within and is responsible for them
class Chunk
{
public:
	//Constructors
	Chunk() = default;
	Chunk(int x, int y, Renderer* ren);
	~Chunk() = default;

	//Functions
	void makeGrid();				//Creates grid to represent chunk borders
	void render();					//Draws grid
	int x() const { return m_IDX; } //Returns x id
	int y() const { return m_IDY; } //Returns y id

	//Object management:
	unsigned char assignObjectToChunk(ObjectPointer object);				//Assigns object to chunk - returns index
	void deassignObjectToChunk(unsigned char index);						//Deassigns object from chunk taking index as value

	//Provide read only access to objects stored in chunk
	const ObjectPointer* chunkObjects() const { return m_ChunkObjects; }

	//Constant for how many objects can exist in a chunk
	static const unsigned char s_OBJECT_COUNT = 24;

private:

	Line m_L1;	//Grid lines
	Line m_L2;

	Renderer* m_Renderer;

	//Chunks contain x many slots for object pointers - 16 right now
	ObjectPointer m_ChunkObjects[s_OBJECT_COUNT] = 
	{
		NULL_OBJECT, NULL_OBJECT, NULL_OBJECT, NULL_OBJECT,
		NULL_OBJECT, NULL_OBJECT, NULL_OBJECT, NULL_OBJECT,
		NULL_OBJECT, NULL_OBJECT, NULL_OBJECT, NULL_OBJECT,
		NULL_OBJECT, NULL_OBJECT, NULL_OBJECT, NULL_OBJECT,
		NULL_OBJECT, NULL_OBJECT, NULL_OBJECT, NULL_OBJECT,
		NULL_OBJECT, NULL_OBJECT, NULL_OBJECT, NULL_OBJECT
	};

	//Chunks are defined by lower left corner
	int m_IDX = 0; 
	int m_IDY = 0;
	float m_XPos = 0;
	float m_YPos = 0;
};

#endif
