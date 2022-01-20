#include "Ship.h"

//Ship
Ship::Ship(float size, Component3f color)
	: m_Ship{ CreateTri(0.0f, 0.0f, size) }, m_Direction{ CreateLine(0.0f,0.0f, 0.0f, 130.0f, 6.0f) }
{
	m_Size = size;
	r = color.a;
	g = color.b;
	b = color.c;
	ColorShape(&m_Ship, r, g, b, Shape::TRI);
	ColorShape(&m_Direction, b, 0.1f, 0.1f, Shape::LINE);
	LayerShape(&m_Direction, LAYER_4, Shape::LINE);
	EngineLog("Ship Created at 0.0, 0.0!");
}

Ship::~Ship() {
	delete m_DeathAnimation;
}

void Ship::render() {
	if (m_Dying) {
		m_DeathAnimation->render();
		return;
	}
	Renderer::commitPrimitive(&m_Ship, GetElementCount(Shape::TRI), Renderer::s_Tri_I, Renderer::IND_TRI);
	Renderer::commitPrimitive(&m_Direction, GetElementCount(Shape::LINE), Renderer::s_Line_I, Renderer::IND_LINE);
}

void Ship::update(double deltaTime) {
	//reset resultant gravity this frame
	m_CurrentGravityResultant = { 0.0f, 0.0f };

	//move with velocity
	translate(m_VelocityX * deltaTime, m_VelocityY * deltaTime);

	//locate current chunk
	m_CurrentChunk = LocateChunk(m_XPos, m_YPos);
}

void Ship::accelerateVelocity(float newVelocityX, float newVelocityY) {
	float magnitudeSpeed = sqrt((newVelocityX * newVelocityX) + (newVelocityY * newVelocityY));
	//Make sure speed isn't excessive
	if (magnitudeSpeed <= m_MaxSpeed) {
		m_VelocityX = newVelocityX;
		m_VelocityY = newVelocityY;
	}
}

void Ship::accelerate(float a) {
	float newVelocityX = m_VelocityX + a * -sin(m_CumulativeAngle);
	float newVelocityY = m_VelocityY + a * cos(m_CumulativeAngle);
	accelerateVelocity(newVelocityX, newVelocityY); //easier for consistency in overloading
}
//when an angle is involved, has to be different as is not relative to ship direction
void Ship::accelerate(float a, float angle) {
	float xAcc = a * cos(angle);
	float yAcc = a * sin(angle);
	float newVelocityX = m_VelocityX + xAcc;
	float newVelocityY = m_VelocityY + yAcc;
	accelerateVelocity(newVelocityX, newVelocityY);

	//as currently just gravity uses this, apply to gravity resultant
	m_CurrentGravityResultant.a += xAcc;
	m_CurrentGravityResultant.b += yAcc;
}

void Ship::brake(double deltaTime) {
	float newVelocityX = m_VelocityX + -(2 * m_VelocityX * deltaTime);
	float newVelocityY = m_VelocityY + -(2 * m_VelocityY * deltaTime);
	float magnitudeSpeed = sqrt((newVelocityX * newVelocityX) + (newVelocityY * newVelocityY));
	//Make sure speed isn't excessive
	if (magnitudeSpeed <= m_MaxSpeed) {
		m_VelocityX = newVelocityX;
		m_VelocityY = newVelocityY;
	}
}

void Ship::resetSpeed() {
	m_VelocityX = 0.0f;
	m_VelocityY = 10.0f;
}

void Ship::translate(float deltaX, float deltaY) {
	TranslateShape(&m_Ship, deltaX, deltaY, Shape::TRI);
	TranslateShape(&m_Direction, deltaX, deltaY, Shape::LINE);
	m_XPos += deltaX;
	m_YPos += deltaY;
}

void Ship::position(float xNew, float yNew) {
	PositionShape(&m_Ship, xNew, yNew, m_XPos, m_YPos, Shape::TRI);
	PositionShape(&m_Direction, xNew, yNew, m_XPos, m_YPos, Shape::LINE);
	m_XPos = xNew;
	m_YPos = yNew;
}

void Ship::rotate(float angle) {
	RotateShape(&m_Ship, m_XPos, m_YPos, angle, Shape::TRI);
	RotateShape(&m_Direction, m_XPos, m_YPos, angle, Shape::LINE);
	m_CumulativeAngle += angle;
}

void Ship::resetRotation() {
	RotateShape(&m_Ship, m_XPos, m_YPos, -m_CumulativeAngle, Shape::TRI);
	RotateShape(&m_Direction, m_XPos, m_YPos, -m_CumulativeAngle, Shape::LINE);
	m_CumulativeAngle = 0.0f;
}

void Ship::timeOfDeath(double time) {
	m_DeathTimer = time;
	m_TimeOfDeath = time + 5.0; //dies 5 seconds after death
	m_Dying = true;

	//color death animation
	m_DeathAnimation = new ShipDeath(m_Size / 2);
	ColorShape(&m_DeathAnimation->m_Tri1, r, g, b, Shape::TRI);
	ColorShape(&m_DeathAnimation->m_Tri2, r, g, b, Shape::TRI);
	ColorShape(&m_DeathAnimation->m_Tri3, r, g, b, Shape::TRI);

	//activate death animation
	m_DeathAnimation->startDeath(xPos(), yPos(), travelDirection(), veloctiy());
	//set renderer for animation
	m_DeathAnimation->setRenderer(m_Renderer);
}

bool Ship::death(double deltaTime) {
	if (m_DeathTimer > m_TimeOfDeath) {
		delete m_DeathAnimation;
		m_DeathAnimation = nullptr; //prevents delete in destructor calling on undefined memory
		return true;
	}
	//add delta time to death
	m_DeathTimer += deltaTime;

	//run animation
	m_DeathAnimation->update(deltaTime);

	m_VelocityX = 0.0f;
	m_VelocityY = 0.0f;
	return false;
}

void Ship::DoTheFunky(float dt) {
	float speed = 2.0f;
	//red
	if (rUp) {
		if (r < 1.0f) {
			r += speed * dt;
		}
		else {
			rUp = false;
		}
	}
	else {
		if (r > 0.0f) {
			r -= speed * dt;
		}
		else {
			rUp = true;
		}
	}
	//green
	if (gUp) {
		if (g < 1.0f) {
			g += speed * dt;
		}
		else {
			gUp = false;
		}
	}
	else {
		if (g > 0.0f) {
			g -= speed * dt;
		}
		else {
			gUp = true;
		}
	}
	//bloo
	if (bUp) {
		if (b < 1.0f) {
			b += speed * dt;
		}
		else {
			bUp = false;
		}
	}
	else {
		if (b > 0.0f) {
			b -= speed * dt;
		}
		else {
			bUp = true;
		}
	}

	ColorShapeVertex(&m_Ship, 0, r, g, b, Shape::TRI);
	ColorShapeVertex(&m_Ship, 1, g, b, r, Shape::TRI);
	ColorShapeVertex(&m_Ship, 2, b, r, g, Shape::TRI);
}

//ship death
ShipDeath::ShipDeath(float size) {
	m_Tri1 = CreateTri(0.0f, 0.0f, size);
	m_Tri2 = CreateTri(0.0f, 0.0f, size);
	m_Tri3 = CreateTri(0.0f, 0.0f, size);

	//rotate
	RotateShape(&m_Tri1, 0.0f, 0.0f, -0.5f, Shape::TRI);
	RotateShape(&m_Tri3, 0.0f, 0.0f, 0.5f, Shape::TRI);
}

void ShipDeath::startDeath(float x, float y, float direction, float speed) {
	PositionShape(&m_Tri1, x, y, 0.0f, 0.0f, Shape::TRI);
	PositionShape(&m_Tri2, x, y, 0.0f, 0.0f, Shape::TRI);
	PositionShape(&m_Tri3, x, y, 0.0f, 0.0f, Shape::TRI);

	m_DeathX = x;
	m_DeathY = y;

	m_Angle = direction;
}

void ShipDeath::render() {
	Renderer::commitPrimitive(&m_Tri1, GetElementCount(Shape::TRI), Renderer::s_Tri_I, Renderer::IND_TRI);
	Renderer::commitPrimitive(&m_Tri2, GetElementCount(Shape::TRI), Renderer::s_Tri_I, Renderer::IND_TRI);
	Renderer::commitPrimitive(&m_Tri3, GetElementCount(Shape::TRI), Renderer::s_Tri_I, Renderer::IND_TRI);
}

void ShipDeath::update(double deltaTime) {

	//move tris
	TranslateShape(&m_Tri1, 8000.0f * deltaTime * cos(m_Angle), 8000.0f * deltaTime * sin(m_Angle), Shape::TRI);
	TranslateShape(&m_Tri2, 8000.0f * deltaTime * cos(m_Angle + 0.5f), 8000.0f * deltaTime * sin(m_Angle + 0.5f), Shape::TRI);
	TranslateShape(&m_Tri3, 8000.0f * deltaTime * cos(m_Angle - 0.5f), 8000.0f * deltaTime * sin(m_Angle - 0.5f), Shape::TRI);
}