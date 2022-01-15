#pragma once
#ifndef OBJECT_MANAGEMENT_H
#define OBJECT_MANAGEMENT_H

//enum to define list object is contained in (star, planet, etc)
enum class ObjectType
{
	//Null - no object in slot
	Null,

	//Vehicles
	Ship,

	//Orbitals
	Star, Planet
};

//Define null object
#define NULL_OBJECT { ObjectType::Null, 0 }

//Struct to contain type of object and location of instructions in list
struct ObjectPointer
{
	ObjectType type;
	unsigned int index; //index
};

#endif