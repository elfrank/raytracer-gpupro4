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

#ifndef __TEXTURE2D_H__
#define __TEXTURE2D_H__

#include <string>

#ifdef WINDOWS
#include "D3DTexture2D.h"
#endif

class Texture2D
{
private:
	D3DTexture2D*		m_pTexture2D;
public:
	Texture2D(void);
	Texture2D(std::string sPath, void* p3DObject)
	{

	}
	~Texture2D(void);

	D3DTexture2D*		GetTexture2D() { return m_pTexture2D; }
	HRESULT				CreateEnvironmentMap( LPCSTR sFileName, ID3D11Device* pDevice ) { return m_pTexture2D->CreateEnvironmentMap(sFileName, pDevice ); }
};

#endif