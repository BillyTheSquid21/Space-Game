#pragma once

#ifndef SPACE_GAME_H
#define SPACE_GAME_H

#include "Game.h"
#include "SpaceGameObjects.h"
#include "World.h"

class SpaceGame : public Game 
{
public:
	//inherits constructors
	using Game::Game;
	~SpaceGame() = default;

	bool init(const char name[], Key_Callback kCallback, Scroll_Callback sCallback);
	void render();
	void update(double deltaTime);
	void handleEvents();
	void handleInput(int key, int scancode, int action, int mods);
	void handleScrolling(double xOffset, double yOffset);

private:
	//can store relevant lists of game objects here
	Ship m_Ship;

	//keeps track of persistent held input
	bool m_HeldInput[5];

	//zoom level for transforming transformations
	float m_ZoomLevel = 1.0f;

	//world
	World m_World;
	//last ship chunk
	ChunkLocation lastShipLocation = { 0,0 };
};

//define where in array held persist is kept
#define HELD_Q m_HeldInput[0]
#define HELD_E m_HeldInput[1]
#define HELD_W m_HeldInput[2]
#define HELD_SPACE m_HeldInput[3]
#define HELD_SHIFT m_HeldInput[4]

#endif
