#include "SpaceGame.h"

bool SpaceGame::init(const char name[], Key_Callback kCallback, Scroll_Callback sCallback) {
	bool initSuccess = Game::init(name, kCallback, sCallback);

	//create ship
	m_Ship = Ship(70.0f);
	m_Ship.setRenderer(&m_Renderer);

	//init world
	m_World.init();
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
	if (HELD_Q) {
		m_Ship.rotate(8.0f * deltaTime);
	}
	if (HELD_E) {
		m_Ship.rotate(-8.0f * deltaTime);
	}
	if (HELD_W) {
		m_Ship.accelerate(600.0f * deltaTime);
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
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		m_Ship.resetRotation();
	}

	//held input
	if (key == GLFW_KEY_Q) {
		if (action == GLFW_PRESS) {
			HELD_Q = true;
		}
		else if (action == GLFW_RELEASE) {
			HELD_Q = false;
		}
	}

	if (key == GLFW_KEY_E) {
		if (action == GLFW_PRESS) {
			HELD_E = true;
		}
		else if (action == GLFW_RELEASE) {
			HELD_E = false;
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

}

void SpaceGame::handleScrolling(double xOffset, double yOffset) {
	if (yOffset > 0 && m_ZoomLevel < 1.0f) {
		m_Renderer.camera.zoomCamera(0.005f);
		m_ZoomLevel += 0.005f;
	}
	else if (yOffset < 0 && m_ZoomLevel > 0.05f) {
		m_Renderer.camera.zoomCamera(-0.005f);
		m_ZoomLevel += -0.005f;
	}
}