#include "Bullets.h"

Bullet::Bullet(float x, float y, float velocity, float angle)
{
	m_Glow = CreateCircle(x, y, BULLET_RADIUS * 3.0f);
	m_Angle = angle;
	m_Velocity = velocity;
	m_XPos = x;
	m_YPos = y;

	//layer shape
	LayerShape(&m_Glow, LAYER_5, Shape::CIRCLE);
	//color shape to green
	ColorShape(&m_Glow, 0.0f, 0.0f, 0.0f, Shape::CIRCLE);
	ColorShapeVertex(&m_Glow, 0, 0.0f, 1.0f, 0.0f, Shape::CIRCLE);
}

void Bullet::update(double deltaTime) {
	TranslateShape(&m_Glow, m_Velocity * cos(m_Angle) * deltaTime, m_Velocity * sin(m_Angle) * deltaTime, Shape::CIRCLE);
	if (m_Green > 0) {
		m_Green -= deltaTime * 0.2f;
	}
	ColorShapeVertex(&m_Glow, 0, 0.0f, m_Green, 0.0f, Shape::CIRCLE);
	m_TimeElapsed += deltaTime;
}

void Bullet::render() {
	Renderer::commitPrimitive(&m_Glow, GetElementCount(Shape::CIRCLE), Renderer::s_Circle_I, Renderer::IND_CIRCLE);
}

bool Bullet::despawn() {
	if (m_TimeElapsed >= LIFESPAN) {
		return true;
	}
	return false;
}

ProjectileManager::ProjectileManager() {
	bullets.resize(MAX_PROJECTILES);
	index.resize(MAX_PROJECTILES);
	for (int i = 0; i < index.size(); i++) {
		index[i] = NULL_OBJECT;
	}
}

void ProjectileManager::addBullet(Bullet bullet) {
	for (unsigned int i = 0; i < MAX_PROJECTILES; i++) {
		if (index[i].type == ObjectType::Null) {
			bullets[i] = bullet;
			index[i] = { ObjectType::Projectile, i};
			return;
		}
	}
	EngineLog("ERROR: TOO MANY PROJECTILES");
}

void ProjectileManager::update(double deltaTime) {
	for (unsigned int i = 0; i < MAX_PROJECTILES; i++) {
		if (!(index[i].type == ObjectType::Null)) {
			if (bullets[i].despawn()) {
				//Frees memory
				index[i].type = ObjectType::Null;
				index[i].index = 0;
				continue;
			}
			bullets[i].update(deltaTime);
		}
	}

}

void ProjectileManager::removeBullet(unsigned int i) {
	index[i].type == ObjectType::Null;
}