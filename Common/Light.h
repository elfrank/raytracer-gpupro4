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

#ifndef __LIGHT__H_
#define __LIGHT__H_

#include "Common.h"
#include "Geometry.h"

class Light
{
public:
	Light( unsigned uiChoice );
	~Light(void);

	void Move( float&, float&, Vector3& );
	void SelectLight(unsigned int);

	Vector3 GetPosition( void ) { return m_vfPosition; }

	void SetPosition( Vector3 &a_vfPosition ) { m_vfPosition = a_vfPosition; }
private:
	Vector3 m_vfPosition;
};

#endif