#pragma once
#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SGUtil.h"

#define Quad std::array<Vertex, 4>
#define Line std::array<Vertex, 4> //Line is just a quad set up to be more convinient
#define Tri std::array<Vertex, 3>
#define Circle std::array<Vertex, 65>

struct Vertex
{
	Component3f position;
	Component3f color;
};

enum class Shape
{
	TRI, QUAD, LINE, CIRCLE
};

//Layer constants - not necessary but makes easier and provides enough layers
const float LAYER_1 = -0.8f;
const float LAYER_2 = -0.6f;
const float LAYER_3 = -0.4f;
const float LAYER_4 = -0.2f;
const float LAYER_5 =  0.0f;
const float LAYER_6 =  0.2f;
const float LAYER_7 =  0.4f;
const float LAYER_8 =  0.6f;
const float LAYER_9 =  0.8f;

//Shape Creation
Quad CreateQuad(float x, float y, float width, float height);
Line CreateLine(float xStart, float yStart, float xEnd, float yEnd, float stroke);
Tri CreateTri(float x, float y, float radius);
Circle CreateCircle(float x, float y, float radius);

//Shape translation - all shapes are defined relative to centre
void TranslateShape(void* verticesArray, float deltaX, float deltaY, Shape type);
void PositionShape(void* verticesArray, float newX, float newY, float currentX, float currentY, Shape type);
void RotateShape(void* verticesArray, float rotationCentreX, float rotationCentreY, float angle, Shape type);
void ColorShape(void* verticesArray, float r, float g, float b, Shape type);
void ColorShapeVertex(void* verticesArray, unsigned int vertex, float r, float g, float b, Shape type);
void LayerShape(void* verticesArray, float layer, Shape type);
void ScaleShape(void* verticesArray, float scale, float centreX, float centreY, Shape type);

//Utility
unsigned short int GetVerticesCount(Shape type);
unsigned short int GetElementCount(Shape type);

#endif
