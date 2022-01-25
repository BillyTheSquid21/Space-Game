#pragma once

#ifndef SPACE_GAME_H
#define SPACE_GAME_H

#include "Game.h"
#include "Ship.h"
#include "World.h"
#include "AI.h"
#include "HUD.h"

class SpaceGame : public Game 
{
public:
	//Inherits constructors
	using Game::Game;
	~SpaceGame() = default;
	
	//Functions
	bool init(const char name[], Key_Callback kCallback, Scroll_Callback sCallback);
	void render();
	void update(double deltaTime);
	void handleEvents();
	void handleInput(int key, int scancode, int action, int mods);
	void handleScrolling(double xOffset, double yOffset);

private:
	//Stores player ship specifically
	Ship m_Player;

	//Enemy ships
	std::vector<Ship> m_EnemyShips;
	std::vector<AI> m_EnemyAI;
	void createEnemy(float xPos, float yPos);
	void setEnemy(unsigned int index);

	//Helper functions
	void createWorld();

	//Keeps track of persistent held input
	bool m_HeldInput[6];

	//Zoom level for transforming transformations
	float m_ZoomLevel = 1.0f;

	//World
	World m_World;

	//Last ship chunk
	ChunkLocation lastShipLocation = { 0,0 };

	//HUD
	HUD m_HUD;
};

//Define where in array held persist is kept
#define HELD_A m_HeldInput[0]
#define HELD_D m_HeldInput[1]
#define HELD_W m_HeldInput[2]
#define HELD_SPACE m_HeldInput[3]
#define HELD_SHIFT m_HeldInput[4]
#define HELD_CTRL m_HeldInput[5]

#endif
