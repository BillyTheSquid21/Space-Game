#include "SpaceGameObjects.h"

//Ship
Ship::Ship(float size) 
	: m_Ship{ CreateTri(0.0f, 0.0f, size) }, m_Direction{CreateLine(0.0f,0.0f, 0.0f, 40.0f, 2.0f)}
{
	r = 0.094f;
	g = 0.584f;
	b = 0.604f;
	ColorShape(&m_Ship, r, g, b, Shape::TRI);
	ColorShape(&m_Direction, b, 0.1f, 0.1f, Shape::LINE);
	LayerShape(&m_Direction, LAYER_4, Shape::LINE);
	EngineLog("Ship Created at 0.0, 0.0!");
}

void Ship::render() {
	Renderer::commitPrimitive(&m_Ship, GetElementCount(Shape::TRI), Renderer::s_TriIndices, Renderer::IND_TRI);
	Renderer::commitPrimitive(&m_Direction, GetElementCount(Shape::LINE), Renderer::s_LineIndices, Renderer::IND_LINE);
}

void Ship::update(double deltaTime) {

	//move with velocity
	translate(m_VelocityX * deltaTime, m_VelocityY * deltaTime);

	//locate current chunk
	m_CurrentChunk = LocateChunk(m_XPos, m_YPos);
}

void Ship::accelerate(float a) {
	float newVelocityX = m_VelocityX + a * -sin(m_CumulativeAngle);
	float newVelocityY = m_VelocityY + a * cos(m_CumulativeAngle);
	float magnitudeSpeed = sqrt((newVelocityX * newVelocityX) + (newVelocityY * newVelocityY));
	//Make sure speed isn't excessive
	if (magnitudeSpeed <= m_MaxSpeed) {
		m_VelocityX = newVelocityX;
		m_VelocityY = newVelocityY;
	}
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

//Orbitals
void Orbital::render() {
	Renderer::commitPrimitive(&m_Orbital, GetElementCount(Shape::CIRCLE), &Renderer::s_CircleIndices, Renderer::IND_CIRCLE);
}

//Star
Star::Star(StarInfo star, unsigned int index) 
{
	//set pointer
	pointer = { ObjectType::Star, index };

	m_XPos = star.x;
	m_YPos = star.y;
	m_Mass = star.mass;

	//work out radius
	m_Radius = sqrt(m_Mass / SG_PI) * 100.0f;

	//make circle
	m_Orbital = CreateCircle(m_XPos, m_YPos, m_Radius);

	//make light bubble
	m_Light = CreateCircle(m_XPos, m_YPos, m_Radius * 10);

	//sets colour
	m_Temperature = star.temp;
	switch (star.temp) 
	{
	case StarColor::RED_DWARF:
		m_Red = 0.82f;
		m_InitialRed = 0.82f;
		m_Green = 0.14f;
		m_Blue = 0.22f;
		m_InitialBlue = 0.22f;
		break;

	case StarColor::SOLAR:
		m_Red = 0.84f;
		m_InitialRed = 0.84f;
		m_Green = 0.73f;
		m_Blue = 0.42f;
		m_InitialBlue = 0.42f;
		break;

	case StarColor::RED_GIANT:
		m_Red = 0.84f;
		m_InitialRed = 0.84f;
		m_Green = 0.13f;
		m_Blue = 0.02f;
		m_InitialBlue = 0.02f;
		break;

	case StarColor::BLUE_GIANT:
		m_Red = 0.34f;
		m_InitialRed = 0.34f;
		m_Green = 0.20f;
		m_Blue = 0.84f;
		m_InitialBlue = 0.84f;
		break;

	default:
		m_Red = 0.0f;
		m_InitialRed = 0.0f;
		m_Green = 1.0f;
		m_Blue = 1.0f;
		m_InitialBlue = 1.0f;
		break;
	}

	ColorShape(&m_Orbital, m_Red, m_Green, m_Blue, Shape::CIRCLE);

	//Light circle
	LayerShape(&m_Light, LAYER_1, Shape::CIRCLE);
	ColorShape(&m_Light, 0.0f, 0.0f, 0.0f, Shape::CIRCLE);
	ColorShapeVertex(&m_Light, 0, 0.2f, 0.2f, 0.2f, Shape::CIRCLE);
}

void Star::update(double deltaTime) {
	if (redUp) {
		m_Red += 0.1f * deltaTime;
		if (m_Red >= m_InitialRed + 0.16) {
			redUp = false;
		}
	}
	else {
		m_Red -= 0.1f * deltaTime;
		if (m_Red <= m_InitialRed - 0.2f) {
			redUp = true;
		}
	}
	if (blueUp) {
		m_Blue += 0.1f * deltaTime;
		if (m_Blue >= m_InitialBlue + 0.16) {
			blueUp = false;
		}
	}
	else {
		m_Blue -= 0.1f * deltaTime;
		if (m_Blue <= m_InitialBlue - 0.2f) {
			blueUp = true;
		}
	}
	ColorShapeVertex(&m_Orbital, 0, m_Red, m_Green, m_Blue, Shape::CIRCLE);
	ColorShapeVertex(&m_Light, 0, 0.2f + m_Red / 4, 0.2f + m_Green / 4, 0.2f + m_Blue / 4, Shape::CIRCLE);
}

void Star::render() {

	Renderer::commitPrimitive(&m_Light, GetElementCount(Shape::CIRCLE), Renderer::s_CircleIndices, Renderer::IND_CIRCLE);

	//inherited
	Orbital::render();
}

//Planets
Planet::Planet(PlanetInfo planet, StarColor parentColor, unsigned int index) {
	//set pointer
	pointer = { ObjectType::Planet, index };

	//x and y pos are dependant on rotation
	m_XPos = 0.0f;
	m_YPos = 0.0f;
	m_RotationX = planet.rotationX;
	m_RotationY = planet.rotationY;
	m_Angle = planet.angle;
	m_Velocity = planet.velocity;
	m_Mass = planet.mass;
	m_OrbitDistance = planet.orbitDistance;

	//work out radius
	m_Radius = sqrt(m_Mass / SG_PI) * 100.0f;

	//make circle
	m_Orbital = CreateCircle(m_RotationX, m_RotationY + m_OrbitDistance, m_Radius);

	//sets type
	m_Type = planet.type;

	//all planets assumed to be created at 12 o clock relative to parent star
	ColorPlanet(m_Type, parentColor, &m_Orbital, m_OrbitDistance);
	
	//rotate planet relative to star
	RotateShape(&m_Orbital, m_RotationX, m_RotationY, m_Angle, Shape::CIRCLE);
}

//Colors planets with lighting depending on parent star - all decided on creation and not modified
void ColorPlanet(PlanetType type, StarColor parentColor, void* orbital, float orbitDistance) {
	//setup color - keep base color <= 0.65 to avoid clamping - and to keep dark
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;

	switch (type) {
	case PlanetType::BLUE_ROCKY:
		r = 0.1f;
		g = 0.3f;
		b = 0.62f;
		break;
	case PlanetType::GREEN_ROCKY:
		r = 0.1f;
		g = 0.62f;
		b = 0.33f;
		break;
	case PlanetType::RED_ROCKY:
		r = 0.64f;
		g = 0.02f;
		b = 0.05f;
		break;
	case PlanetType::GREY_ROCKY:
		r = 0.57f;
		g = 0.54f;
		b = 0.58f;
		break;
	case PlanetType::BLUE_GAS:
		r = 0.13f;
		g = 0.23f;
		b = 0.65f;
		break;
	case PlanetType::RED_GAS:
		r = 0.63f;
		g = 0.13f;
		b = 0.02f;
		break;
	case PlanetType::GREEN_GAS:
		r = 0.03f;
		g = 0.64f;
		b = 0.15f;
		break;
	case PlanetType::YELLOW_GAS:
		r = 0.03f;
		g = 0.65f;
		b = 0.62f;
	default:
		r = 0.5f;
		g = 0.0f;
		b = 0.5f;
		break;
	}

	//adjust front face lighting - max change +0.35 - if followed should be in range and avoid clamping
	float rOff = 0.0f;
	float gOff = 0.0f;
	float bOff = 0.0f;

	//adds offset and divides by orbit distance / min distance for max lighting
	//distanceFactor = m_OrbitDistance / min distance - should be one below min;
	float distanceFactor = 0.1f;

	switch (parentColor) {
	case StarColor::RED_DWARF:
		distanceFactor = orbitDistance / MINIMUM_ORBIT_RED_DWARF;
		if (distanceFactor < 1) {
			distanceFactor = 1;
		}
		rOff = 0.30f / distanceFactor;
		break;
	case StarColor::SOLAR:
		distanceFactor = orbitDistance / MINIMUM_ORBIT_SOLAR;
		if (distanceFactor < 1) {
			distanceFactor = 1;
		}
		rOff = 0.21f / distanceFactor;
		gOff = 0.25f / distanceFactor;
		bOff = 0.13f / distanceFactor;
		break;
	case StarColor::RED_GIANT:
		distanceFactor = orbitDistance / MINIMUM_ORBIT_RED_GIANT;
		if (distanceFactor < 1) {
			distanceFactor = 1;
		}
		rOff = 0.35f / distanceFactor;
		gOff = 0.12f / distanceFactor;
		break;
	case StarColor::BLUE_GIANT:
		distanceFactor = orbitDistance / MINIMUM_ORBIT_BLUE_GIANT;
		if (distanceFactor < 1) {
			distanceFactor = 1;
		}
		gOff = 0.11f / distanceFactor;
		bOff = 0.35f / distanceFactor;
		break;
	}

	//color with brighter values
	ColorShape(orbital, r + rOff, g + gOff, b + bOff, Shape::CIRCLE);

	//color first 32 vertices in base color
	for (int i = 1; i < 33; i++) {
		ColorShapeVertex(orbital, i, r, g, b, Shape::CIRCLE);
	}
}

void Planet::update(double deltaTime) {
	m_Angle += deltaTime * m_Velocity;
	RotateShape(&m_Orbital, m_RotationX, m_RotationY, deltaTime * m_Velocity, Shape::CIRCLE);
	
	//update xPos and yPos from cumulative
	//Make relative to centre
	float x1 = m_XPos - m_RotationX;
	float y1 = m_YPos - m_RotationY;

	//Apply rotation
	float tempX1 = x1 * cos(m_Angle) - y1 * sin(m_Angle);
	float tempY1 = x1 * sin(m_Angle) + y1 * cos(m_Angle);

	//set location
	m_XPos = tempX1 + m_RotationX;
	m_YPos = tempY1 + m_RotationY;
}

PlanetType ReturnRandomPlanet() 
{
	int random = rand() % PLANET_MAX_PROBABILITY;

	//must be in order
	if (random < s_PlanetProbabilities.BLUE_ROCKY) {
		return PlanetType::BLUE_ROCKY;
	}
	else if (random < s_PlanetProbabilities.RED_ROCKY) {
		return PlanetType::RED_ROCKY;
	}
	else if (random < s_PlanetProbabilities.GREEN_ROCKY) {
		return PlanetType::GREEN_ROCKY;
	}
	else if (random < s_PlanetProbabilities.GREY_ROCKY) {
		return PlanetType::GREY_ROCKY;
	}
	else if (random < s_PlanetProbabilities.BLUE_GAS) {
		return PlanetType::BLUE_GAS;
	}
	else if (random < s_PlanetProbabilities.RED_GAS) {
		return PlanetType::RED_GAS;
	}
	else if (random < s_PlanetProbabilities.GREEN_GAS) {
		return PlanetType::GREEN_GAS;
	}
	else if (random < s_PlanetProbabilities.YELLOW_GAS) {
		return PlanetType::YELLOW_GAS;
	}
	//defaults to
	return PlanetType::BLUE_ROCKY;
}
