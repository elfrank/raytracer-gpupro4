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

#include "MaterialData.h"

MaterialData::MaterialData(void)
{
	m_fDiffuse[0] = m_fDiffuse[1] = m_fDiffuse[2] = 0.5f; m_fDiffuse[3] = 0.8f;
	m_fAmbient[0] = m_fAmbient[1] = m_fAmbient[2] = 0.0f; m_fAmbient[3] = 1.0f;
	m_fSpecular[0] = m_fSpecular[1] = m_fSpecular[2] = 0.8f; m_fSpecular[3] = 1.0f;
	m_fAlpha = 0.f;
	m_fShininess = 0.f;
}

MaterialData::~MaterialData(void)
{
}
