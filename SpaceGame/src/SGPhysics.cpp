#include "SGPhysics.h"

float CalculateDistance(float x1, float y1, float x2, float y2) {
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

Force CalculateGravity(float x1, float y1, float x2, float y2, float mass) {
	float distance = CalculateDistance(x1, y1, x2, y2);
	float direction = atan2((y2 - y1), (x2 - x1));

	if (direction < 0.0f)
	{
		direction += 2 * SG_PI;
	}

	return { SG_PHYS_G * mass / (distance), direction};
}
