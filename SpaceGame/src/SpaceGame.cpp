#include "SpaceGame.h"

bool SpaceGame::init(const char name[], Key_Callback kCallback, Scroll_Callback sCallback) {
	bool initSuccess = Game::init(name, kCallback, sCallback);

	//create ship
	m_Ship = Ship(100.0f);
	m_Ship.setRenderer(&m_Renderer);

	//init world
	m_World.init(&m_Ship);
	m_World.setRenderer(&m_Renderer);
	m_World.initialGenerateChunks();

	//init renderer
	m_Renderer.camera.setZoomCamera(0.1f);
	m_ZoomLevel = 0.1;

	return initSuccess;
}

void SpaceGame::render() {
	//commit primitives and check bounds and stuff here
	m_Ship.render();
	m_World.render();
	Game::render(); //call at bottom to inherit method
}

float angle = 0.0f;
void SpaceGame::update(double deltaTime) {

	//world update
	if (lastShipLocation.x != m_Ship.location().x || lastShipLocation.y != m_Ship.location().y) {
		m_World.manageChunks(m_Ship.location());
		lastShipLocation = m_Ship.location();
	}

	m_World.update(deltaTime);

	//update position and things here

	//ship
	if (HELD_A) {
		m_Ship.rotate(6.0f * deltaTime);
	}
	if (HELD_D) {
		m_Ship.rotate(-6.0f * deltaTime);
	}
	if (HELD_W) {
		m_Ship.accelerate(2200.0f * deltaTime);
	}
	if (HELD_SPACE) {
		m_Ship.brake(deltaTime);
	}
	//keeps ship pointing in direction of motion
	if (HELD_SHIFT) {
		m_Ship.resetRotation();
		m_Ship.rotate(-m_Ship.travelDirection());
	}
	m_Ship.update(deltaTime);

	//camera
	m_Renderer.camera.positionCamera(-m_Ship.xPos() * m_ZoomLevel, -m_Ship.yPos() * m_ZoomLevel);

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
		m_Ship.resetRotation();
		m_Ship.rotate(-m_Ship.travelDirection());
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