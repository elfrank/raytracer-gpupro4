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
// Object.h
// ------------------------------------------------
// Object properties of a scene

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Common.h"
#include "Geometry.h"
#include "Vertex.h"
#include "Matrix4.h"
#include "Model.h"

// Instancing models
class Object
{
private:
	Matrix4			m_World; // Position of the instance
	Model*			m_Model; // Pointer to the model
	bool			m_bIsDynamic;
public:

	Object(Model* a_Model, Point &a_Position);
	~Object(void);

	Model*			GetModel() { return m_Model; }
	Vertex*			GetVertices() { return m_Model->GetVertices(); }
	unsigned int	GetNumPrimitives() { return m_Model->GetNumPrimitives(); }
	unsigned int	GetNumVertices() { return m_Model->GetNumVertices(); }
	Primitive**		GetPrimitives() { return m_Model->GetPrimitives(); }
	string			GetName()	{ return m_Model->GetName(); }
	Matrix4			GetWorldMatrix() { return m_World; }
	bool			IsDynamic() { return m_bIsDynamic; }
};

#endif