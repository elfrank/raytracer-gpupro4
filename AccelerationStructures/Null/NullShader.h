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

#ifndef NULLSHADER_H
#define NULLSHADER_H

#include "AccelerationStructure.h"
#include "BBox.h"

class NullShader : public AccelerationStructure
{
private:
public:
	NullShader(Primitive** a_Primitive = NULL, unsigned int a_NumPrimitives = 0);
	~NullShader();

	TIntersection IntersectP(Ray &a_Ray);

	void						Build() {}
	void						PrintOutput(float &tiempototal);
	char*						GetName( void ) { return m_sName; }
	unsigned int				GetNumberOfElements() { return 0; }
	size_t						GetSizeOfElement() { return 0; }
	void*						GetPtrToElements() { return static_cast<void*>(NULL); }
	unsigned int*				GetOrderedElementsIds() { return 0; }
	unsigned int				GetKeyboardId() { return 4;}
	BBox						WorldBound() { return BBox(); }
};

#endif