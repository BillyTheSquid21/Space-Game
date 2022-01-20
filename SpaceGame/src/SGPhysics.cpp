#include "SGPhysics.h"

float CalculateDistance(float x1, float y1, float x2, float y2) {
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

Force CalculateGravity(float x1, float y1, float x2, float y2, float mass) {
	float distance = CalculateDistance(x1, y1, x2, y2);
	float direction = CalculateAngle(x1, y1, x2, y2);

	return { SG_PHYS_G * mass / (distance), direction};
}

float CalculateAngle(float x1, float y1, float x2, float y2) {
	float direction = atan2((y2 - y1), (x2 - x1));

	if (direction < 0.0f)
	{
		direction += 2 * SG_PI;
	}
	return direction;
}

bool CircleCollision(void* polygon, unsigned int verticeCount, float xCircle, float yCircle, float radius) {
	Vertex* verticePointer = (Vertex*)polygon;
	//if any vertice is within radius, has collided
	for (int i = 0; i < verticeCount; i++) {
		float distance = CalculateDistance(verticePointer[i].position.a, verticePointer[i].position.b,
			xCircle, yCircle);
		if (distance < radius) {
			return true;
		}
	}
	return false;
}
