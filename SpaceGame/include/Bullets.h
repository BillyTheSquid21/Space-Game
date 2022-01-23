#pragma once
#ifndef BULLETS_H
#define BULLETS_H

#include "GameObject.h"
#include "ObjectManagement.h"

const float BULLET_RADIUS = 16.0f;
const double LIFESPAN = 5.0f;

class Bullet : public GameObject
{
public:
	Bullet() = default;
	Bullet(float x, float y, float velocity, float angle);
	~Bullet() = default;

	void update(double deltaTime);
	void render();
	bool despawn();

	//Getters
	Component3f bullet() const { return {m_Glow[0].position.a, m_Glow[0].position.b, BULLET_RADIUS }; }

private:

	//Kinematics
	float m_Angle = 0.0f;
	float m_Velocity = 0;

	//Time
	float m_TimeElapsed = 0.0f;

	//Draw
	Circle m_Glow;
	float m_Green = 1.0f;
};

const unsigned int MAX_PROJECTILES = 2048;

//Manages projectiles to avoid constant memory allocation
class ProjectileManager
{
public:
	ProjectileManager();
	~ProjectileManager() = default;
	void addBullet(Bullet bullet);
	void removeBullet(unsigned int i);
	void update(double deltaTime);
	std::vector<Bullet> bullets;
	std::vector<ObjectPointer> index;
};

#endif