#include "SpaceGame.h"

bool SpaceGame::init(const char name[], Key_Callback kCallback, Scroll_Callback sCallback) {
	bool initSuccess = Game::init(name, kCallback, sCallback);

	//create ship
	m_Player = Ship(100.0f);
	m_Player.setRenderer(&m_Renderer);

	//init world
	m_World.init(&m_Player);
	m_World.setRenderer(&m_Renderer);
	m_World.initialGenerateChunks();

	//init renderer
	m_Renderer.camera.setZoomCamera(0.1f);
	m_ZoomLevel = 0.1;

	return initSuccess;
}

void SpaceGame::render() {
	//commit primitives and check bounds and stuff here
	m_Player.render();
	m_World.render();
	Game::render(); //call at bottom to inherit method
}

float angle = 0.0f;
void SpaceGame::update(double deltaTime) {

	//world update
	if (lastShipLocation.x != m_Player.location().x || lastShipLocation.y != m_Player.location().y) {
		m_World.manageChunks(m_Player.location());
		lastShipLocation = m_Player.location();
	}

	//if update returns false, reset world
	if (!m_World.update(deltaTime, m_GlfwTime)) {

		//create new ship
		m_Player = Ship(100.0f);
		m_Player.setRenderer(&m_Renderer);

		//create new world
		m_World = World();
		
		//init world
		m_World.init(&m_Player);
		m_World.setRenderer(&m_Renderer);
		m_World.initialGenerateChunks();
	}

	//update position and things here

	//ship
	if (HELD_A) {
		m_Player.rotate(6.0f * deltaTime);
	}
	if (HELD_D) {
		m_Player.rotate(-6.0f * deltaTime);
	}
	if (HELD_W) {
		m_Player.accelerate(2200.0f * deltaTime);
	}
	if (HELD_SPACE) {
		m_Player.brake(deltaTime);
	}
	//keeps ship pointing in direction of motion
	if (HELD_SHIFT) {
		m_Player.resetRotation();
		m_Player.rotate(-m_Player.travelDirection());
	}
	m_Player.update(deltaTime);

	//camera
	m_Renderer.camera.positionCamera(-m_Player.xPos() * m_ZoomLevel, -m_Player.yPos() * m_ZoomLevel);

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
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		m_Player.resetRotation();
		m_Player.rotate(-m_Player.travelDirection());
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