#include "Orbitals.h"

//Orbitals
void Orbital::render() {
	Renderer::commitPrimitive(&m_Orbital, GetElementCount(Shape::CIRCLE), &Renderer::s_Circle_I, Renderer::IND_CIRCLE);
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
	case StarType::RED_DWARF:
		m_Red = 0.82f;
		m_InitialRed = 0.82f;
		m_Green = 0.14f;
		m_Blue = 0.22f;
		m_InitialBlue = 0.22f;
		break;

	case StarType::SOLAR:
		m_Red = 0.84f;
		m_InitialRed = 0.84f;
		m_Green = 0.73f;
		m_Blue = 0.42f;
		m_InitialBlue = 0.42f;
		break;

	case StarType::RED_GIANT:
		m_Red = 0.84f;
		m_InitialRed = 0.84f;
		m_Green = 0.13f;
		m_Blue = 0.02f;
		m_InitialBlue = 0.02f;
		break;

	case StarType::BLUE_GIANT:
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

	Renderer::commitPrimitive(&m_Light, GetElementCount(Shape::CIRCLE), Renderer::s_Circle_I, Renderer::IND_CIRCLE);

	//inherited
	Orbital::render();
}

StarType ReturnRandomStar() {
	int random = rand() % STAR_MAX_PROBABILITY;

	//must be in order
	if (random < s_StarProbabilities.RED_DWARF) {
		return StarType::RED_DWARF;
	}
	else if (random < s_StarProbabilities.SOLAR) {
		return StarType::SOLAR;
	}
	else if (random < s_StarProbabilities.RED_GIANT) {
		return StarType::RED_GIANT;
	}
	else if (random < s_StarProbabilities.BLUE_GIANT) {
		return StarType::BLUE_GIANT;
	}
	//defaults to
	return StarType::RED_DWARF;
}

float GetRandomStarMass(StarType type) {
	switch (type) {
	case StarType::RED_DWARF:
		return (float)(rand() % 60) + 30.0f;
	case StarType::SOLAR:
		return (float)(rand() % 100) + 80.0f;
	case StarType::RED_GIANT:
		return (float)(rand() % 130) + 130.0f;
	case StarType::BLUE_GIANT:
		return (float)(rand() % 300) + 180.0f;
	}
	return 100.0f; //default
}

float GetRandomOrbitDistance(StarType type) {
	switch (type)
	{
	case StarType::RED_DWARF:
		return MINIMUM_ORBIT_RED_DWARF + (rand() % 200);
	case StarType::SOLAR:
		return MINIMUM_ORBIT_SOLAR + (rand() % 200);
	case StarType::RED_GIANT:
		return MINIMUM_ORBIT_RED_GIANT + (rand() % 200);
	case StarType::BLUE_GIANT:
		return MINIMUM_ORBIT_BLUE_GIANT + (rand() % 200);
	}
	return MINIMUM_ORBIT_RED_DWARF; //default
}

//Planets
Planet::Planet(PlanetInfo planet, StarType parentColor, unsigned int index) {
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
void ColorPlanet(PlanetType type, StarType parentColor, void* orbital, float orbitDistance) {
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
	case StarType::RED_DWARF:
		distanceFactor = orbitDistance / MINIMUM_ORBIT_RED_DWARF;
		if (distanceFactor < 1) {
			distanceFactor = 1;
		}
		rOff = 0.30f / distanceFactor;
		break;
	case StarType::SOLAR:
		distanceFactor = orbitDistance / MINIMUM_ORBIT_SOLAR;
		if (distanceFactor < 1) {
			distanceFactor = 1;
		}
		rOff = 0.21f / distanceFactor;
		gOff = 0.25f / distanceFactor;
		bOff = 0.13f / distanceFactor;
		break;
	case StarType::RED_GIANT:
		distanceFactor = orbitDistance / MINIMUM_ORBIT_RED_GIANT;
		if (distanceFactor < 1) {
			distanceFactor = 1;
		}
		rOff = 0.35f / distanceFactor;
		gOff = 0.12f / distanceFactor;
		break;
	case StarType::BLUE_GIANT:
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

	//set location from orbital centre
	m_XPos = m_Orbital[0].position.a;
	m_YPos = m_Orbital[0].position.b;
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
