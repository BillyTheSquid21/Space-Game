#include "SpaceGame.h"

bool SpaceGame::init(const char name[], Key_Callback kCallback, Scroll_Callback sCallback) {
	bool initSuccess = Game::init(name, kCallback, sCallback);

	//init renderer
	m_Renderer.camera.setZoomCamera(0.1f);
	m_ZoomLevel = 0.1f;

	//scale gui relative to resultion
	float scaleFactor = m_Width / 1920.0f;
	m_Renderer.gui.setScale(scaleFactor);

	//make world
	createWorld();

	return initSuccess;
}

void SpaceGame::render() {
	//commit primitives and check bounds and stuff here
	m_Player.render();
	m_World.render();

	//update ships
	for (int i = 0; i < m_EnemyShips.size(); i++) {
		m_EnemyShips[i].render();
	}

	//Render HUD
	m_HUD.render();

	Game::render(); //call at bottom to inherit method
}

float angle = 0.0f;
void SpaceGame::update(double deltaTime) {

	//ship
	if (HELD_A && !m_Player.dying()) {
		float rotation = 6.0f * deltaTime;
		m_Player.rotate(rotation);
		m_HUD.rotateDirectionFacing(rotation);
	}
	if (HELD_D && !m_Player.dying()) {
		float rotation = -6.0f * deltaTime;
		m_Player.rotate(rotation);
		m_HUD.rotateDirectionFacing(rotation);
	}
	if (HELD_W) {
		m_Player.accelerate(2200.0f * deltaTime);
	}
	if (HELD_SPACE) {
		m_Player.brake(deltaTime);
	}
	//keeps ship pointing in direction of motion
	if (HELD_SHIFT && !m_Player.dying()) {
		m_Player.resetRotation();
		m_Player.rotate(m_Player.travelDirection() - SG_PI / 2); //Ship is off by 90 deg as relative to vertical axis
		m_HUD.resetFacing();
		m_HUD.rotateDirectionFacing(m_Player.travelDirection() - SG_PI / 2);
	}
	if (HELD_CTRL) {
		m_Player.shoot();
	}
	m_Player.update(deltaTime);

	//world update
	if (lastShipLocation.x != m_Player.location().x || lastShipLocation.y != m_Player.location().y) {
		m_World.manageChunks(m_Player.location());
		lastShipLocation = m_Player.location();
	}

	//if update returns false, reset world
	if (!m_World.update(deltaTime, m_GlfwTime)) {
		//create new world
		createWorld();
	}

	//camera
	m_Renderer.camera.positionCamera(-m_Player.xPos() * m_ZoomLevel, -m_Player.yPos() * m_ZoomLevel);

	//update ships
	for (int i = 0; i < m_EnemyShips.size(); i++) {
		m_EnemyShips[i].update(deltaTime);
		m_EnemyAI[i].update(deltaTime);
		
		//death animation
		if (m_EnemyShips[i].dying()) {
			if (m_EnemyShips[i].death(deltaTime) && !m_Player.dying()) {
				setEnemy(i);
			}
		}
	}

	//update direction moving hud
	m_HUD.resetMoving();
	m_HUD.rotateDirectionMoving(m_Player.travelDirection() - SG_PI / 2);
	
	//inherits
	Game::update(deltaTime);
}

//Event and control handling
void SpaceGame::handleEvents() {
	Game::handleEvents();
}

void SpaceGame::handleInput(int key, int scancode, int action, int mods) {
	Game::handleInput(key, scancode, action, mods);

	//put single button directly in here
	if (key == GLFW_KEY_F && action == GLFW_PRESS && !m_Player.dying()) {
		m_Player.resetRotation();
		m_Player.rotate(-m_Player.travelDirection());
		m_HUD.resetFacing();
	}

	//held input
	if (key == GLFW_KEY_SPACE) {
		if (action == GLFW_PRESS) {
			HELD_SPACE = true;
		}
		else if (action == GLFW_RELEASE) {
			HELD_SPACE = false;
		}
	}

	if (key == GLFW_KEY_A) {
		if (action == GLFW_PRESS) {
			HELD_A = true;
		}
		else if (action == GLFW_RELEASE) {
			HELD_A = false;
		}
	}

	if (key == GLFW_KEY_D) {
		if (action == GLFW_PRESS) {
			HELD_D = true;
		}
		else if (action == GLFW_RELEASE) {
			HELD_D = false;
		}
	}

	if (key == GLFW_KEY_W) {
		if (action == GLFW_PRESS) {
			HELD_W = true;
		}
		else if (action == GLFW_RELEASE) {
			HELD_W = false;
		}
	}

	if (key == GLFW_KEY_LEFT_SHIFT) {
		if (action == GLFW_PRESS) {
			HELD_SHIFT = true;
		}
		else if (action == GLFW_RELEASE) {
			HELD_SHIFT = false;
		}
	}

	if (key == GLFW_KEY_LEFT_CONTROL) {
		if (action == GLFW_PRESS) {
			m_Player.shoot();
			HELD_CTRL = true;
		}
		else if (action == GLFW_RELEASE) {
			HELD_CTRL = false;
		}
	}
}

void SpaceGame::handleScrolling(double xOffset, double yOffset) {
	if (yOffset > 0 && m_ZoomLevel < 0.5f) {
		m_Renderer.camera.zoomCamera(0.005f);
		m_ZoomLevel += 0.005f;
	}
	else if (yOffset < 0 && m_ZoomLevel > 0.025f) {
		m_Renderer.camera.zoomCamera(-0.005f);
		m_ZoomLevel += -0.005f;
	}
}

void SpaceGame::createEnemy(float xPos, float yPos) {
	//allocate ship and set properties
	Ship tempShipPtr = Ship::Ship(100.0f, { 0.92f, 0.12f, 0.22f });
	tempShipPtr.setRenderer(&m_Renderer);
	tempShipPtr.setMaxSpeed(15000.0f);
	tempShipPtr.position(xPos, yPos);

	m_EnemyShips.push_back(tempShipPtr);
	m_World.addShip(&m_EnemyShips[m_EnemyShips.size() - 1]);

	//make ai
	AI tempAIPtr = AI::AI(&m_Player, &m_EnemyShips[m_EnemyShips.size() - 1]);
	tempAIPtr.setWorld(&m_World);
	m_EnemyAI.push_back(tempAIPtr);

	//set all previous ai
	for (int i = 0; i < m_EnemyShips.size() - 1; i++) {
		AI tempAIPtr = AI::AI(&m_Player, &m_EnemyShips[i]);
		tempAIPtr.setWorld(&m_World);
		m_EnemyAI[i] = tempAIPtr;
	}

	//set all previous worlds
	for (int i = 0; i < m_EnemyShips.size() - 1; i++) {
		m_World.setShip(&m_EnemyShips[i], i);
	}
}

void SpaceGame::setEnemy(unsigned int index) {

	//allocate ship and set properties
	Ship tempShipPtr = Ship::Ship(100.0f, { 0.92f, 0.12f, 0.22f });
	tempShipPtr.setRenderer(&m_Renderer);
	tempShipPtr.setMaxSpeed(15000.0f);

	m_EnemyShips[index] = tempShipPtr;
	m_World.addShip(&m_EnemyShips[index]);

	//make ai
	AI tempAIPtr = AI::AI(&m_Player, &m_EnemyShips[index]);
	tempAIPtr.setWorld(&m_World);
	m_EnemyAI[index] = tempAIPtr;
}

void SpaceGame::createWorld() {
	//clear enemies
	m_EnemyShips.clear();
	m_EnemyAI.clear();

	//create new ship
	m_Player = Ship(100.0f, { 0.094f, 0.584f, 0.604f });
	m_Player.setRenderer(&m_Renderer);

	//create new world
	m_World = World();

	//init world
	m_World.init();
	m_World.setRenderer(&m_Renderer);
	m_World.initialGenerateChunks();

	//add ships to world
	m_World.addShip(&m_Player);


	//add enemy ships
	createEnemy(-800.0f, -16000.0f);
	createEnemy(800.0f, -16000.0f);

	//reset HUD
	m_HUD.resetFacing();
}