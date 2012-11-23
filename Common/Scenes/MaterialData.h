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

#ifndef __MATERIALDATA_H__
#define __MATERIALDATA_H__

#include <stdio.h>
#include <string.h>

class MaterialData
{
private:
	float			m_fDiffuse[4]; // diffuse rate
	float			m_fAmbient[4];
	float			m_fSpecular[4];
	float			m_fShininess;
	float			m_fAlpha;
	unsigned int	uiMaterialId;
	unsigned int	m_uiPadding;
public:
	MaterialData();
	~MaterialData(void);

	float*			GetDiffuse( void ) { return &m_fDiffuse[0]; }
	float*			GetAmbient( void ) { return &m_fAmbient[0]; }
	float*			GetSpecular( void ) { return &m_fSpecular[0]; }
	float			GetShininess( void ) { return m_fShininess; }
	float			GetAlpha( void ) { return m_fAlpha; }

	void			SetDiffuse( const float *fDiffuse ) { memcpy( &m_fDiffuse, &fDiffuse[0], sizeof(fDiffuse[0])*4 ); }
	void			SetAmbient( const float *fAmbient ) { memcpy( &m_fAmbient, &fAmbient[0], sizeof(fAmbient[0])*4 ); }
	void			SetSpecular( const float *fSpecular ) { memcpy( &m_fSpecular, &fSpecular[0], sizeof(fSpecular[0])*4 ); }
	void			SetAlpha( const float fAlpha ) { m_fAlpha = fAlpha; }
	void			SetShininess( const float fShininess ) { m_fShininess = fShininess; }
};

#endif