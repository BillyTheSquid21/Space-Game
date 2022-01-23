#include "World.h"

//world
void World::init() {
	//allocate vector
	m_ChunkList.resize(CHUNK_CYCLES_TOTAL);
	m_ChunkIDCache.resize(CHUNK_CYCLES_TOTAL);

	Ship ship = Ship::Ship();
	WorldShip shipTemp = { &ship, nullptr, 1000000.0f, nullptr, 1000000.0f };
	m_WorldShips.push_back(shipTemp);
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
				if (m_StarsList[j].pointer.index != chunk.chunkObjects()[i].index) {
					continue;
				}
				//update instructions
				m_StarsInstructionsList[m_StarsList[j].pointer.index] = m_StarsList[j].unload();

				//remove from rendering list
				m_StarsList.erase(m_StarsList.begin() + j);
				break;
			}
			break;
		case ObjectType::Planet:
			for (int j = 0; j < m_PlanetsList.size(); j++) {
				if (m_PlanetsList[j].pointer.index != chunk.chunkObjects()[i].index) {
					continue;
				}
				//update instructions
				m_PlanetsInstructionsList[m_PlanetsList[j].pointer.index] = m_PlanetsList[j].unload();

				//remove from rendering list
				m_PlanetsList.erase(m_PlanetsList.begin() + j);
				break;
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
				if (m_ChunkIDCache[i].x != m_UnloadedChunks[j].x() || m_ChunkIDCache[i].y != m_UnloadedChunks[j].y()) {
					continue;
				}
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

	//Sees if a star should be generated
	checkStarGen();

	//Assign new array at end
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
	//draw projectiles
	for (int i = 0; i < MAX_PROJECTILES; i++) {
		if (m_ProjectileManager.index[i].type != ObjectType::Null) {
			m_ProjectileManager.bullets[i].render();
		}
	}
}

bool World::update(double deltaTime, double time) {

	//Updates all physics objects, both static and dynamic
	updatePhysicsBodies(deltaTime, time);

	//Applies gravity
	applyGravity();

	//Handles ai death and removes dead ships if player not dying, game can then rebuild ship and add
	aiDeath();

	//deals with player death and returns whether to reset world
	return playerDeath(deltaTime);
}

void World::updatePhysicsBodies(double deltaTime, double time) {
	//set all distances for closest objects to max and stars to nullptr
	resetShipClosestPhysicsBodies();

	//update stars and influences
	updatePhysicsStars(deltaTime, time);

	//update planets and influences
	updatePhysicsPlanets(deltaTime, time);

	//update bullets and collision
	updatePhysicsBullets(deltaTime, time);
}

void World::updatePhysicsBullets(double deltaTime, double time) {
	//update bullets
	for (int i = 0; i < m_WorldShips.size(); i++) {
		//check if ship has shot
		if (m_WorldShips[i].ship->hasShot() && !m_WorldShips[i].ship->dying()) {
			float direction = m_WorldShips[i].ship->direction() + SG_PI / 2;
			float xPos = m_WorldShips[i].ship->xPos() + 200.0f * cos(direction);
			float yPos = m_WorldShips[i].ship->yPos() + 200.0f * sin(direction);
			Bullet bullet = Bullet::Bullet(xPos, yPos, m_WorldShips[i].ship->veloctiy() + 4500.0f, direction);
			bullet.setRenderer(m_Renderer);
			m_ProjectileManager.addBullet(bullet);
		}
		//check if ship has been shot
		for (int j = 0; j < MAX_PROJECTILES; j++) {
			if (m_ProjectileManager.index[j].type == ObjectType::Null) {
				continue;
			}
			Component3f bulletProperties = m_ProjectileManager.bullets[j].bullet();
			if (CircleCollision(m_WorldShips[i].ship->xPos(), m_WorldShips[i].ship->yPos(), 80.0f,
				bulletProperties.a, bulletProperties.b, bulletProperties.c)) {
				m_ProjectileManager.removeBullet(j);
				EngineLog("HIT: ", i);
				if (m_WorldShips[i].ship->hit() && !m_WorldShips[i].ship->dying()) {
					m_WorldShips[i].ship->timeOfDeath(time);
				}
			}
		}
	}
	m_ProjectileManager.update(deltaTime);
}

void World::updatePhysicsStars(double deltaTime, double time) {
	//update stars
	for (int i = 0; i < m_StarsList.size(); i++) {
		//update star
		m_StarsList[i].update(deltaTime);

		for (int shipCount = 0; shipCount < m_WorldShips.size(); shipCount++) {
			//Check if collision has happened
			if (PolygonCircleCollision(m_WorldShips[shipCount].ship->triangle(), GetVerticesCount(Shape::TRI),
				m_StarsList[i].xPos(), m_StarsList[i].yPos(), m_StarsList[i].radius())) {

				//Do collision here
				if (!m_WorldShips[shipCount].ship->dying()) {
					m_WorldShips[shipCount].ship->timeOfDeath(time);
				}
			}

			//see what star is closest
			float distance = CalculateDistance(m_WorldShips[shipCount].ship->xPos(), m_WorldShips[shipCount].ship->yPos(),
				m_StarsList[i].xPos(), m_StarsList[i].yPos());

			if (distance < m_WorldShips[shipCount].closestStarDistance) {
				m_WorldShips[shipCount].closestStarDistance = distance;
				m_WorldShips[shipCount].closestStar = &m_StarsList[i];
			}
		}
	}
}

void World::updatePhysicsPlanets(double deltaTime, double time) {
	//update planets and find closest to ship
	for (int i = 0; i < m_PlanetsList.size(); i++) {
		//update planet
		m_PlanetsList[i].update(deltaTime);

		for (int shipCount = 0; shipCount < m_WorldShips.size(); shipCount++) {

			//Check if collision has happened
			if (PolygonCircleCollision(m_WorldShips[shipCount].ship->triangle(), GetVerticesCount(Shape::TRI),
				m_PlanetsList[i].xPos(), m_PlanetsList[i].yPos(), m_PlanetsList[i].radius())) {

				//do collision here
				if (!m_WorldShips[shipCount].ship->dying()) {
					m_WorldShips[shipCount].ship->timeOfDeath(time);
				}
			}

			//see what planet is closest for gravity
			float distance = CalculateDistance(m_WorldShips[shipCount].ship->xPos(),
				m_WorldShips[shipCount].ship->yPos(), m_PlanetsList[i].xPos(), m_PlanetsList[i].yPos());
			if (distance < m_WorldShips[shipCount].closestPlanetDistance) {
				m_WorldShips[shipCount].closestPlanetDistance = distance;
				m_WorldShips[shipCount].closestPlanet = &m_PlanetsList[i];
			}
		}
	}
}

void World::resetShipClosestPhysicsBodies() {
	//set all distances for closest objects to max and stars to nullptr
	for (int i = 0; i < m_WorldShips.size(); i++) {
		m_WorldShips[i].closestPlanet = nullptr;
		m_WorldShips[i].closestPlanetDistance = 1000000.0f;
		m_WorldShips[i].closestStar = nullptr;
		m_WorldShips[i].closestStarDistance = 1000000.0f;
	}
}

void World::aiDeath() {
	//checks if ships are dead before removing them
	for (int i = 2; i < m_WorldShips.size(); i++) {
		if (m_WorldShips[i].ship->death() && !m_WorldShips[1].ship->dying()) {
			m_WorldShips.erase(m_WorldShips.begin() + i);
			i--;
		}
	}
}

bool World::playerDeath(double deltaTime) {
	//updates dying animation for ship at index 0 - location of player
	if (m_WorldShips[1].ship->dying()) {
		if (m_WorldShips[1].ship->death(deltaTime)) {
			gameOver();
			return false;
		}
	}
	return true;
}

void World::applyGravity() {
	for (int i = 0; i < m_WorldShips.size(); i++) {
		//apply gravity for closest planet
		if (m_WorldShips[i].closestPlanet != NULL) {
			Force gravity = CalculateGravity(m_WorldShips[i].ship->xPos(), m_WorldShips[i].ship->yPos(),
				m_WorldShips[i].closestPlanet->xPos(), m_WorldShips[i].closestPlanet->yPos(), 
				m_WorldShips[i].closestPlanet->mass());
			m_WorldShips[i].ship->accelerate(gravity.magnitude, gravity.direction);
		}
		//apply gravity for closest star
		if (m_WorldShips[i].closestStar != NULL) {
			Force gravity = CalculateGravity(m_WorldShips[i].ship->xPos(), m_WorldShips[i].ship->yPos(),
				m_WorldShips[i].closestStar->xPos(), m_WorldShips[i].closestStar->yPos(), 
				m_WorldShips[i].closestStar->mass());
			m_WorldShips[i].ship->accelerate(gravity.magnitude, gravity.direction);
		}
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
	angle = m_WorldShips[0].ship->direction();

	//randomise velocity - between 0.5 units and 10
	velocity = ((float)(rand() % 100000) / 100000.0f) + 0.00001f;

	//assign and push
	PlanetInfo planetInfo = { mass, angle, velocity, type, parent->xPos(), parent->yPos(), orbitDistance };
	Planet planetObject = Planet(planetInfo, parent->type(), m_PlanetsInstructionsList.size());
	chunk.assignObjectToChunk(planetObject.pointer);
	m_PlanetsList.push_back(planetObject);
	m_PlanetsInstructionsList.push_back(planetInfo);
}

void World::gameOver()
{
	//set ship position to origin
	m_WorldShips[0].ship->position(0.0f, 0.0f);
	m_WorldShips[0].ship->resetRotation();

	//set speed to 0
	m_WorldShips[0].ship->resetSpeed();
}

//Add ship
void World::addShip(Ship* ship) {
	WorldShip shipData = { ship, nullptr, 1000000.0f, nullptr, 1000000.0f };
	m_WorldShips.push_back(shipData);
}

void World::setShip(Ship* ship, unsigned int index) {
	WorldShip shipData = { ship, nullptr, 1000000.0f, nullptr, 1000000.0f };
	m_WorldShips[index] = shipData;
}

bool World::shipsInSync(unsigned int size) {
	EngineLog("Game: ", size);
	EngineLog("World: ", m_WorldShips.size());
	if (size <= m_WorldShips.size()) {
		return true;
	}
	return false;
}