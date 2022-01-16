#include "SGPhysics.h"

float CalculateDistance(float x1, float y1, float x2, float y2) {
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

Force CalculateGravity(float x1, float y1, float x2, float y2, float mass) {
	//find distance r
	float r = CalculateDistance(x1, y1, x2, y2);

	if (r < 100.0f) {
		r = 100.0f;
	}

	EngineLog("DISTANCE: ", r);

	//find magnitude
	float mag = (SG_PHYS_G * mass) / (r);

	EngineLog("MAG: ", mag);

	//find direction from x1,y1
	float direction = atan((x2 - x1) / (y2 - y1));

	return { mag, direction };
}