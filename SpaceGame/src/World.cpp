#include "World.h"

//world
void World::init(Ship* player) {
	//allocate vector
	m_ChunkList.resize(CHUNK_CYCLES_TOTAL);
	m_ChunkIDCache.resize(CHUNK_CYCLES_TOTAL);

	//set pointer for player
	m_PlayerPointer = player;
}

void World::setRenderer(Renderer* ren) {
	m_Renderer = ren;
}

void World::initialGenerateChunks() 
{	
	//for checking amount gen
	unsigned int total = 0;

	//index to insert
	unsigned int index = 0;

	//create top right
	for (int x = 0; x < DRAW_LIMIT; x++) {
		for (int y = 0; y < DRAW_LIMIT; y++) {
			Chunk chunk = Chunk(x, y, m_Renderer);
			chunk.makeGrid();
			m_ChunkList[index] = chunk;
			index++;
			m_ChunkIDList.push_back({ x,y });
			total++;
		}
	}

	//create top left
	for (int x = -1; x > -1 * DRAW_LIMIT; x--) {
		for (int y = 0; y < DRAW_LIMIT; y++) {
			Chunk chunk = Chunk(x, y, m_Renderer);
			chunk.makeGrid();
			m_ChunkList[index] = chunk;
			index++;
			m_ChunkIDList.push_back({ x,y });
			total++;
		}
	}

	//create bottom right
	for (int x = 0; x < DRAW_LIMIT; x++) {
		for (int y = -1; y > -1 * DRAW_LIMIT; y--) {
			Chunk chunk = Chunk(x, y, m_Renderer);
			chunk.makeGrid();
			m_ChunkList[index] = chunk;
			index++;
			m_ChunkIDList.push_back({ x,y });
			total++;
		}
	}

	//create bottom left
	for (int x = -1; x > -1 * DRAW_LIMIT; x--) {
		for (int y = -1; y > -1 * DRAW_LIMIT; y--) {
			Chunk chunk = Chunk(x, y, m_Renderer);
			chunk.makeGrid();
			m_ChunkList[index] = chunk;
			index++;
			m_ChunkIDList.push_back({ x,y });
			total++;
		}
	}

	EngineLog("Total chunks initialised: ", total);
}

//unloads object assigned to chunk
void World::unloadChunkObjects(Chunk& chunk) {
	//unload all objects assigned to chunk
	for (int i = 0; i < Chunk::s_OBJECT_COUNT; i++) {
		if (chunk.chunkObjects()[i].type == ObjectType::Null) {
			continue;
		}

		//If isn't null, check type to deal with unloading accordingly
		switch (chunk.chunkObjects()[i].type) {
		case ObjectType::Star:
			for (int j = 0; j < m_StarsList.size(); j++) {
				if (m_StarsList[j].pointer.index == chunk.chunkObjects()[i].index) {
					//update instructions
					m_StarsInstructionsList[m_StarsList[j].pointer.index] = m_StarsList[j].unload();

					//remove from rendering list
					m_StarsList.erase(m_StarsList.begin() + j);
					break;
				}
			}
			break;
		case ObjectType::Planet:
			for (int j = 0; j < m_PlanetsList.size(); j++) {
				if (m_PlanetsList[j].pointer.index == chunk.chunkObjects()[i].index) {
					//update instructions
					m_PlanetsInstructionsList[m_PlanetsList[j].pointer.index] = m_PlanetsList[j].unload();

					//remove from rendering list
					m_PlanetsList.erase(m_PlanetsList.begin() + j);
					break;
				}
			}
			break;

		}
	}
}

void World::loadChunkObjects(Chunk& chunk) {
	//unload all objects assigned to chunk
	for (int i = 0; i < Chunk::s_OBJECT_COUNT; i++) {
		if (chunk.chunkObjects()[i].type == ObjectType::Null) {
			continue;
		}

		//if isn't null, load instructions for constructing
		unsigned int index = 0;
		switch (chunk.chunkObjects()[i].type) {
		case ObjectType::Star:
		{
			//loads instructions from index
			index = chunk.chunkObjects()[i].index;
			Star star = Star(m_StarsInstructionsList[chunk.chunkObjects()[i].index], index);
			m_StarsList.push_back(star);
		}
		break;
		case ObjectType::Planet:
		{
			//loads instructions from index - assumes last star added is for this planet
			//this assumption is as a planet can only generate after a parent star
			//and is assigned to the same chunk and NEVER DEASSIGNED
			//if it fails it'll just look weird
			index = chunk.chunkObjects()[i].index;
			Planet planet = Planet(m_PlanetsInstructionsList[chunk.chunkObjects()[i].index], m_StarsList[m_StarsList.size() - 1].type(), index);
			m_PlanetsList.push_back(planet);
		}
		break;
		}
	}
}

void World::generateChunkRange(ChunkLocation originChunk) {
	//Create array for all ids as always generates this many chunk ids
	unsigned short int index = 0;

	int leftBound = originChunk.x - DRAW_LIMIT;
	int rightBound = originChunk.x + DRAW_LIMIT;
	int topBound = originChunk.y + DRAW_LIMIT;
	int bottomBound = originChunk.y - DRAW_LIMIT;

	//create top right
	for (int x = originChunk.x; x < rightBound; x++) {
		for (int y = originChunk.y; y < topBound; y++) {
			m_ChunkIDCache[index] = { x,y };
			index++;
		}
	}

	//create top left
	for (int x = originChunk.x - 1; x > leftBound; x--) {
		for (int y = originChunk.y; y < topBound; y++) {
			m_ChunkIDCache[index] = { x,y };
			index++;
		}
	}

	//create bottom right
	for (int x = originChunk.x; x < rightBound; x++) {
		for (int y = originChunk.y - 1; y > bottomBound; y--) {
			m_ChunkIDCache[index] = { x,y };
			index++;
		}
	}

	//create bottom left
	for (int x = originChunk.x - 1; x > leftBound; x--) {
		for (int y = originChunk.y - 1; y > bottomBound; y--) {
			m_ChunkIDCache[index] = { x,y };
			index++;
		}
	}
}

void World::manageChunks(ChunkLocation originChunk) 
{	
	
	//Bounds
	int leftBound = originChunk.x - DRAW_LIMIT;
	int rightBound = originChunk.x + DRAW_LIMIT;
	int topBound = originChunk.y + DRAW_LIMIT;
	int bottomBound = originChunk.y - DRAW_LIMIT;

	//Generates chunk ids within range
	generateChunkRange(originChunk);

	//create new array of chunks
	std::vector<Chunk> newChunkList;
	newChunkList.resize(CHUNK_CYCLES_TOTAL);

	//carry over any chunks currently loaded that stay loaded - always less than or eq to max
	int newListIndex = 0;
	for (int i = 0; i < CHUNK_CYCLES_TOTAL; i++) {
		if (m_ChunkList[i].x() > leftBound && m_ChunkList[i].x() < rightBound &&
			m_ChunkList[i].y() > bottomBound && m_ChunkList[i].y() < topBound) {
			newChunkList[newListIndex] = m_ChunkList[i];
			newListIndex++;
		}
		else {
			//else store in unloaded
			m_UnloadedChunks.push_back(m_ChunkList[i]);

			//unload objects
			unloadChunkObjects(m_ChunkList[i]);
		}
	}

	//check if chunk exists, if not, create
	for (int i = 0; i < CHUNK_CYCLES_TOTAL; i++) {
		bool chunkExists = false;
		for (int j = 0; j < m_ChunkIDList.size(); j++) {
			if (m_ChunkIDCache[i].x == m_ChunkIDList[j].x && m_ChunkIDCache[i].y == m_ChunkIDList[j].y) {
				chunkExists = true;
				break;
			}
		}

		if (!(newListIndex < CHUNK_CYCLES_TOTAL)) {
			break;
		}

		//if doesn't exist, create new chunk
		else if (!chunkExists) {
			Chunk chunk = Chunk(m_ChunkIDCache[i].x, m_ChunkIDCache[i].y, m_Renderer);
			chunk.makeGrid();
			newChunkList[newListIndex] = chunk;

			if (m_GenStar) {
				generateSolarSystem(m_ChunkIDCache[i].x, m_ChunkIDCache[i].y, newChunkList[newListIndex]);
			}

			//add id
			m_ChunkIDList.push_back(m_ChunkIDCache[i]);
			newListIndex++;
		}

		else {	//else load from unloaded chunks
			for (int j = 0; j < m_UnloadedChunks.size(); j++) {
				if (m_ChunkIDCache[i].x == m_UnloadedChunks[j].x() && m_ChunkIDCache[i].y == m_UnloadedChunks[j].y()) {
					newChunkList[newListIndex] = m_UnloadedChunks[j];

					//load in objects
					loadChunkObjects(newChunkList[newListIndex]);

					newListIndex++;

					//remove from unloaded
					m_UnloadedChunks.erase(m_UnloadedChunks.begin() + j);
					break;
				}
			}
		}
	}

	checkStarGen();

	//assign new array at end
	m_ChunkList = newChunkList;
}

void World::checkStarGen() {
	//stops star from spawning until x many chunk cycles - should stop from being next to each other
	if (!m_GenStar) {
		m_StarGenCount++;
		if (m_StarGenCount >= 6) {
			m_GenStar = true;
		}
	}
}

void World::render() {
	//draw chunk grids
	for (int i = 0; i < CHUNK_CYCLES_TOTAL; i++) {
		m_ChunkList[i].render();
	}
	//draw stars
	for (int i = 0; i < m_StarsList.size(); i++) {
		m_StarsList[i].render();
	}
	//draw planets
	for (int i = 0; i < m_PlanetsList.size(); i++) {
		m_PlanetsList[i].render();
	}
}

void World::update(double deltaTime) {

	//update stars
	Star* closestStar = nullptr;
	float closestStarDistance = 1000000.0f;
	for (int i = 0; i < m_StarsList.size(); i++) {
		//update star
		m_StarsList[i].update(deltaTime);

		//Check if collision has happened
		if (CircleCollision(m_PlayerPointer->triangle(), GetVerticesCount(Shape::TRI),
			m_StarsList[i].xPos(), m_StarsList[i].yPos(), m_StarsList[i].radius())) {
			
			//do collision here
		}

		//see what star is closest
		float distance = CalculateDistance(m_PlayerPointer->xPos(), m_PlayerPointer->yPos(),
			m_StarsList[i].xPos(), m_StarsList[i].yPos());
		if (distance < closestStarDistance) {
			closestStarDistance = distance;
			closestStar = &m_StarsList[i];
		}
	}

	//update planets and find closest to ship
	Planet* closestPlanet = nullptr;
	float closestPlanetDistance = 1000000.0f;
	for (int i = 0; i < m_PlanetsList.size(); i++) {
		//update planet
		m_PlanetsList[i].update(deltaTime);

		//Check if collision has happened
		if (CircleCollision(m_PlayerPointer->triangle(), GetVerticesCount(Shape::TRI),
			m_StarsList[i].xPos(), m_PlanetsList[i].yPos(), m_PlanetsList[i].radius())) {

			//do collision here
		}

		//see what planet is closest
		float distance = CalculateDistance(m_PlayerPointer->xPos(), m_PlayerPointer->yPos(),
			m_PlanetsList[i].xPos(), m_PlanetsList[i].yPos());
		if (distance < closestPlanetDistance) {
			closestPlanetDistance = distance;
			closestPlanet = &m_PlanetsList[i];
		}
	}

	//apply gravity for closest planet
	if (closestPlanet != NULL) {
		Force gravity = CalculateGravity(m_PlayerPointer->xPos(), m_PlayerPointer->yPos(),
			closestPlanet->xPos(), closestPlanet->yPos(), closestPlanet->mass());
		m_PlayerPointer->accelerate(gravity.magnitude, gravity.direction);
	}
	//apply gravity for closest star
	if (closestStar != NULL) {
		Force gravity = CalculateGravity(m_PlayerPointer->xPos(), m_PlayerPointer->yPos(),
			closestStar->xPos(), closestStar->yPos(), closestStar->mass());
		m_PlayerPointer->accelerate(gravity.magnitude, gravity.direction);
	}

}

void World::generateSolarSystem(int x, int y, Chunk& chunk) {
	//have chance of creating star - planets are bound to origin chunk
	int random = (rand() % 2000) + 1;
	if (random < 400) {

		StarInfo starInfo = {};

		//pick type
		StarType type = ReturnRandomStar();

		//pick mass
		float mass = GetRandomStarMass(type);

		starInfo = { x * CHUNK_TO_WORLD_FACTOR, y * CHUNK_TO_WORLD_FACTOR, mass, type };

		//add to star list
		Star starObject = Star(starInfo, m_StarsInstructionsList.size());
		m_StarsList.push_back(starObject);

		//add to starinfo list
		m_StarsInstructionsList.push_back(starInfo);

		//assign to chunk
		chunk.assignObjectToChunk(starObject.pointer);

		//create up to 9 planets
		int planetCount = (rand() % 9) + 1;
		float distanceFromStar = GetRandomOrbitDistance(starObject.type());
		//make each planet, incrementing orbit distance
		for (int i = 0; i < planetCount; i++) {
			generatePlanet(&starObject, chunk, distanceFromStar);
			distanceFromStar += (rand() % 2200) + 4800.0f;
		}

		m_GenStar = false;
		m_StarGenCount = 0;
	}
}

//generate Planets
void World::generatePlanet(Star* parent, Chunk& chunk, float orbitDistance) 
{	
	//initialise values to assign to planet
	PlanetType type = ReturnRandomPlanet();
	float mass = 1.0f;
	float angle = 0.0f;
	float velocity = 0.0f;

	//generate mass - Grey rocky is last rocky planet and all rocky and gas planets have same mass probs
	if ((int)type < (int)(PlanetType::GREY_ROCKY)) {
		mass = (float)(rand() % 25) + 5.0f;
	}
	else {
		mass = (float)(rand() % 55) + 30.0f;
	}

	//make angle facing away from player - means planets wont suddenly pop in in front of player
	angle = m_PlayerPointer->direction();

	//randomise velocity - between 0.5 units and 10
	velocity = ((float)(rand() % 100000) / 100000.0f) + 0.00001f;

	//assign and push
	PlanetInfo planetInfo = { mass, angle, velocity, type, parent->xPos(), parent->yPos(), orbitDistance };
	Planet planetObject = Planet(planetInfo, parent->type(), m_PlanetsInstructionsList.size());
	chunk.assignObjectToChunk(planetObject.pointer);
	m_PlanetsList.push_back(planetObject);
	m_PlanetsInstructionsList.push_back(planetInfo);
}