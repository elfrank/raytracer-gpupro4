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

#include "Light.h"

Light::Light( unsigned int uiChoice )
{
	SelectLight(uiChoice);
}

Light::~Light(void)
{
}

void Light::SelectLight( unsigned int uiChoice)
{
	switch(uiChoice)
	{
	case 0:
		m_vfPosition = Vector3(1.732050807568877/4.0,2.0/4.0,-3.0/4.0);
		break;
	case 1:
		m_vfPosition = Vector3(0.f,0.2f,0.f);
		break;
	}
}

void Light::Move( float &a_fTime, float &a_fSpeed, Vector3 &a_vTraslation )
{
	float NormSQ;
	Dot(NormSQ, a_vTraslation, a_vTraslation);
	if(NormSQ > 0.1f)
	{
		// Move Light
		a_vTraslation = (-a_fTime*a_fSpeed/sqrt(NormSQ)) * a_vTraslation;
		m_vfPosition += a_vTraslation;
	}
}
