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

#ifndef __D3DTEXTURE2D_H__
#define __D3DTEXTURE2D_H__

#include <D3DX11tex.h>
#include <D3DX11.h>
#include <D3DX11core.h>
#include <D3DX11async.h>
#include <D3Dcompiler.h>

class D3DTexture2D
{
private:
	ID3D11Texture2D*			m_pTexture;
    ID3D11ShaderResourceView*	m_pSRV;
public:
	D3DTexture2D(void);
	~D3DTexture2D(void);

	ID3D11ShaderResourceView*	GetSRV() { return m_pSRV; }
	ID3D11Texture2D*			GetTexture2D() { return m_pTexture; }

	HRESULT						CreateEnvironmentMap( LPCSTR sFileName, ID3D11Device* pDevice );
	HRESULT						CreateTexture2D( UINT iNumElements, ID3D11Resource** pResource, const char* pInitData, ID3D11Device* pDevice );
};

#endif