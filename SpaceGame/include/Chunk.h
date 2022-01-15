#pragma once
#ifndef CHUNK_H
#define CHUNK_H

#include "Renderer.h"
#include "ObjectManagement.h"

const float CHUNK_TO_WORLD_FACTOR = 9820.0f; //2560 coords in 1 chunk
const unsigned char DRAW_LIMIT = 3;

//Keeps track of how many chunks are generated in one cycle
constexpr unsigned int CHUNK_CYCLES_TOTAL = ((DRAW_LIMIT * 2) - 1) * ((DRAW_LIMIT * 2) - 1);

//Define minimum distance a planet can spawn to a star
const float MINIMUM_ORBIT_RED_DWARF = 2000.0f;
const float MINIMUM_ORBIT_SOLAR = 3300.0f;
const float MINIMUM_ORBIT_RED_GIANT = 3800.0f;
const float MINIMUM_ORBIT_BLUE_GIANT = 4000.0f;

struct ChunkLocation
{
	int x;
	int y;
};
//find chunk id of current object
ChunkLocation LocateChunk(float xPos, float yPos);

class Chunk
{
public:
	Chunk() = default;
	Chunk(int x, int y, Renderer* ren);
	~Chunk() = default;

	void makeGrid();
	void render();
	int x() const { return m_IDX; }
	int y() const { return m_IDY; }

	//Object management
	//Assigns object to chunk - returns index
	unsigned char assignObjectToChunk(ObjectPointer object);
	void deassignObjectToChunk(unsigned char index);

	//provide read only access to objects
	const ObjectPointer* chunkObjects() const { return m_ChunkObjects; }

	static const unsigned char s_OBJECT_COUNT = 16;

private:
	Line l1;
	Line l2;

	Renderer* m_Renderer;

	//Chunks contain x many slots for object pointers - 16 right now
	ObjectPointer m_ChunkObjects[s_OBJECT_COUNT] = 
	{
		NULL_OBJECT, NULL_OBJECT, NULL_OBJECT, NULL_OBJECT,
		NULL_OBJECT, NULL_OBJECT, NULL_OBJECT, NULL_OBJECT,
		NULL_OBJECT, NULL_OBJECT, NULL_OBJECT, NULL_OBJECT,
		NULL_OBJECT, NULL_OBJECT, NULL_OBJECT, NULL_OBJECT
	};

	int m_IDX = 0; //chunks are defined by lower left corner
	int m_IDY = 0;
	float m_XPos = 0;
	float m_YPos = 0;
};

#endif
