// ================================================================================ //
// The authors of this code (Francisco Ávila, Sergio Murguía, Leo Reyes and Arturo	//
// García) hereby release it into the public domain. This applies worldwide.		//
//																					//
// In case this is not legally possible, we grant any entity the right to use this	//
// work for any purpose, without any conditions, unless such conditions are			//
// required by law.																	//
//																					//
// This work is provided on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF	//
// ANY KIND. You are solely responsible for the consequences of using or			//
// redistributing this code and assume any risks associated with these or related	//
// activities.																		//
// ================================================================================ //

// ------------------------------------------------
// Vertex.h
// ------------------------------------------------
// It defines the properties of a Vertex.

#ifndef VERTEX_H
#define VERTEX_H

#include "Geometry.h"

struct Vertex
{
    Point Pos;				// Position
	Vector Normal;			// Normal
	float U, V;

	Vertex(Point pp) : Pos(pp) {  }
	Vertex(Point pp, Vector nn) : Pos(pp), Normal(nn), U(0), V(0) {}
	Vertex(float xx = 0.0f, float yy = 0.0f, float zz = 0.0f) { Pos.x=xx;Pos.y=yy;Pos.z=zz;}

	void PrintString()
	{
		printf("Pos");
		Pos.PrintString();
		printf(" Normal");
		Normal.PrintString();
		printf(" UV");
		printf("(%f,%f)",U,V);
	}
};

#endif