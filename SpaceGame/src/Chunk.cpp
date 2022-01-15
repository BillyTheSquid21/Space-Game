#include "Chunk.h"

ChunkLocation LocateChunk(float xPos, float yPos) {
	return { intFloor(xPos / CHUNK_TO_WORLD_FACTOR), intFloor(yPos / CHUNK_TO_WORLD_FACTOR) };
}

Chunk::Chunk(int x, int y, Renderer* ren)
	: m_XPos{ x * CHUNK_TO_WORLD_FACTOR }, m_YPos{ y * CHUNK_TO_WORLD_FACTOR },
	m_IDX{ x }, m_IDY{ y }, m_Renderer{ ren }
{

}

void Chunk::makeGrid() {
	//create grid
	l1 = CreateLine(m_XPos, m_YPos, m_XPos, m_YPos + CHUNK_TO_WORLD_FACTOR, 1.4f);
	l2 = CreateLine(m_XPos, m_YPos, m_XPos + CHUNK_TO_WORLD_FACTOR, m_YPos, 1.4f);
	LayerShape(&l1, LAYER_1, Shape::LINE);
	LayerShape(&l2, LAYER_1, Shape::LINE);
}

void Chunk::render() {
	if (m_Renderer->isInBounds(&l1, GetVerticesCount(Shape::LINE))) {
		Renderer::commitPrimitive(&l1, GetElementCount(Shape::LINE),
			Renderer::s_LineIndices, Renderer::IND_LINE);
	}

	if (m_Renderer->isInBounds(&l2, GetVerticesCount(Shape::LINE))) {
		Renderer::commitPrimitive(&l2, GetElementCount(Shape::LINE),
			Renderer::s_LineIndices, Renderer::IND_LINE);
	}
}

//Object management
unsigned char Chunk::assignObjectToChunk(ObjectPointer object) {
	//search through list for space
	for (int i = 0; i < s_OBJECT_COUNT; i++) {
		if (m_ChunkObjects[i].type == ObjectType::Null) {
			m_ChunkObjects[i] = object;
			return i;
		}
	}

	//if gets to end
	EngineLog("Chunk has run out of object slots! ", m_IDX, m_IDY);
	return s_OBJECT_COUNT; //returns number at max - tells object to handle error
}

void Chunk::deassignObjectToChunk(unsigned char index) {
	m_ChunkObjects[index] = NULL_OBJECT;
}