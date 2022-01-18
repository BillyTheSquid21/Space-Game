#pragma once
#ifndef GAME_H
#define GAME_H

#include "Renderer.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <sstream>
#include "SGUtil.h"
#include "GameObject.h"

//callback pointer definition
typedef void (*Key_Callback)(GLFWwindow* window, int key, int scancode, int action, int mods);
typedef void (*Scroll_Callback)(GLFWwindow* window, double xoffset, double yoffset);

class Game 
{

public:

	//Default constructor and destructor
	Game() = default;
	~Game() = default;

	//Main constructor
	Game(int width, int height);

	//Primary functions
	bool init(const char name[], Key_Callback kCallback, Scroll_Callback sCallback);
	void handleEvents();
	void handleInput(int key, int scancode, int action, int mods);
	void handleScrolling(double xOffset, double yOffset);
	void update(double deltaTime);
	void setTime(double time);
	void render();
	void clean();

	//Secondary Functions
	double secondsPerFrameCap() const { return m_SecondsPerFrameCap; }

	//window
	GLFWwindow* window = NULL;

protected:

	int m_Width;
	int m_Height;

	double m_GlfwTime;

	double m_SecondsPerFrameCap = (1.0 / 500.0) * 1000.0; //Init to 500, can be changed

	//render
	Shader m_ShaderProgram;
	Renderer m_Renderer;
	
};
#endif