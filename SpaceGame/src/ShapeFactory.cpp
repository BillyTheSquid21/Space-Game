#include "ShapeFactory.h"

//Creation
Quad CreateQuad(float x, float y, float width, float height) {


	Vertex v0;
	v0.position = { x - (width / 2), y - (height / 2),  LAYER_5 };
	v0.color = { 1.0f, 0.0f, 1.0f };

	Vertex v1;
	v1.position = { x + (width / 2), y - (height / 2),  LAYER_5 };
	v1.color = { 1.0f, 0.0f, 1.0f };

	Vertex v2;
	v2.position = { x + (width / 2), y + (height / 2),  LAYER_5 };
	v2.color = { 1.0f, 0.0f, 1.0f };

	Vertex v3;
	v3.position = { x - (width / 2), y + (height / 2),  LAYER_5 };
	v3.color = { 1.0f, 0.0f, 1.0f };

	return { v0, v1, v2, v3 };
}

//Creation
Line CreateLine(float xStart, float yStart, float xEnd, float yEnd, float stroke) {

	//Use perpendicular method
	float xScale = yEnd - yStart;
	float yScale = xEnd - xStart;
	float magnitude = invSqrt((xScale * xScale) + (yScale * yScale));
	xScale *= -1 * magnitude; yScale *= magnitude;

	Vertex v0;
	v0.position = { xStart - (xScale * stroke), yStart - (yScale * stroke),  LAYER_2 };
	v0.color = { 0.20f, 0.20f, 0.22f };

	Vertex v1;
	v1.position = { xStart + (xScale * stroke), yStart + (yScale * stroke),  LAYER_2 };
	v1.color = { 0.20f, 0.20f, 0.22f };

	Vertex v2;
	v2.position = { xEnd + (xScale * stroke), yEnd + (yScale * stroke),  LAYER_2 };
	v2.color = { 0.20f, 0.20f, 0.22f };

	Vertex v3;
	v3.position = { xEnd - (xScale * stroke), yEnd - (yScale * stroke),  LAYER_2 };
	v3.color = { 0.20f, 0.20f, 0.22f };

	return { v0, v1, v2, v3 };
}

//Creates upright equilateral triangle with approx radian values
Tri CreateTri(float x, float y, float radius) {

	Vertex v0;
	v0.position = { (radius * cos(1.5708f)) + x, (radius * sin(1.5708f)) + y,  LAYER_5 };
	v0.color = { 1.0f, 0.0f, 1.0f };

	Vertex v1;
	v1.position = { (radius * cos(3.6652f)) + x, (radius * sin(3.6652f)) + y,  LAYER_5 };
	v1.color = { 1.0f, 0.0f, 1.0f };

	Vertex v2;
	v2.position = { (radius * cos(5.7596f)) + x, (radius * sin(5.7596f)) + y,  LAYER_5 };
	v2.color = { 1.0f, 0.0f, 1.0f };

	return { v0, v1, v2};
}

Circle CreateCircle(float x, float y, float radius) {

	//init array
	Circle circle;

	//Make Centre
	Vertex v0;
	v0.position = { x, y, LAYER_5 };
	v0.color = { 1.0f, 0.0f, 1.0f };

	circle[0] = v0;

	//Make 64 concentric vertices
	float angle = 0.0f;
	for (int i = 1; i < 65; i++) {
		Vertex vI;
		vI.position = { (radius * cos(angle)) + x, (radius * sin(angle)) + y,  LAYER_5 };
		vI.color = { 1.0f, 0.0f, 1.0f };
		angle += 0.0998f; //angle increase by approx 360/64 in radians 
		circle[i] = vI;
	}

	return circle;
}

unsigned short int GetVerticesCount(Shape type) {
	unsigned short int numberOfVertices;
	switch (type)
	{
	case Shape::TRI:
		numberOfVertices = 3;
		break;
	case Shape::QUAD:
		numberOfVertices = 4;
		break;
	case Shape::LINE:
		numberOfVertices = 4;
		break;
	case Shape::CIRCLE:
		numberOfVertices = 65;
		break;
	default:
		numberOfVertices = 0;
		break;
	}
	return numberOfVertices;
}

unsigned short int GetElementCount(Shape type) {
	return 6 * GetVerticesCount(type);
}

//Translation
void TranslateShape(void* verticesArray, float deltaX, float deltaY, Shape type) 
{	
	Vertex* vertexPointer = (Vertex*)verticesArray;

	//Set number of vertices to translate
	unsigned short int numberOfVertices = GetVerticesCount(type);

	//Translate for each vertice
	for (int i = 0; i < numberOfVertices; i++) {
		vertexPointer[i].position.a += deltaX;
		vertexPointer[i].position.b += deltaY;
	}
}

//Position
void PositionShape(void* verticesArray, float newX, float newY, float currentX, float currentY, Shape type)
{
	//get amount to translate by
	float deltaX = newX - currentX;
	float deltaY = newY - currentY;

	TranslateShape(verticesArray, deltaX, deltaY, type);
}

//Rotation
void RotateShape(void* verticesArray, float rotationCentreX, float rotationCentreY, float angle, Shape type) 
{
	Vertex* vertexPointer = (Vertex*)verticesArray;

	//Set number of vertices to translate
	unsigned short int numberOfVertices = GetVerticesCount(type);

	//Translate for each vertice
	for (int i = 0; i < numberOfVertices; i++) {
		//Make relative to centre
		float x1 = vertexPointer[i].position.a - rotationCentreX;
		float y1 = vertexPointer[i].position.b - rotationCentreY;
		
		//Apply rotation
		float tempX1 = x1 * cos(angle) - y1 * sin(angle);
		float tempY1 = x1 * sin(angle) + y1 * cos(angle);

		//set location
		vertexPointer[i].position.a = tempX1 + rotationCentreX;
		vertexPointer[i].position.b = tempY1 + rotationCentreY;
	}
}

//put on layer - stick to whole numbers personally to save confusion
void LayerShape(void* verticesArray, float layer, Shape type) {
	Vertex* vertexPointer = (Vertex*)verticesArray;

	//Set number of vertices to translate
	unsigned short int numberOfVertices = GetVerticesCount(type);

	//Translate for each vertice
	for (int i = 0; i < numberOfVertices; i++) {
		vertexPointer[i].position.c = layer;
	}
}

//Color

//Set color of single vertex
static void ColorVertex(Vertex* vertexPointer, unsigned int vertex, float r, float g, float b) {
	vertexPointer[vertex].color.a = r;
	vertexPointer[vertex].color.b = g;
	vertexPointer[vertex].color.c = b;
}

//Set color of whole shape
void ColorShape(void* verticesArray, float r, float g, float b, Shape type)
{
	Vertex* vertexPointer = (Vertex*)verticesArray;

	//Set number of vertices to translate
	unsigned short int numberOfVertices = GetVerticesCount(type);

	//Color for each vertice
	for (int i = 0; i < numberOfVertices; i++) {
		ColorVertex(vertexPointer, i, r, g, b);
	}
}

//Set color of single vertex (with check for users to make sure vertex exists)
void ColorShapeVertex(void* verticesArray, unsigned int vertex, float r, float g, float b, Shape type) {
	if (vertex < GetVerticesCount(type)) {
		Vertex* vertexPointer = (Vertex*)verticesArray;
		ColorVertex(vertexPointer, vertex, r, g, b);
		return;
	}
	EngineLog("Error: Shape doesn't have vertice at specified location");
}