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

#ifndef __D3DCONSTANTBUFFER_H__
#define __D3DCONSTANTBUFFER_H__

#include <D3DX11tex.h>
#include <D3DX11.h>
#include <D3DX11core.h>
#include <D3DX11async.h>
#include <D3Dcompiler.h>

class D3D11ConstantBuffer
{
private:
	ID3D11Buffer*		m_pBuffer;
	char				m_cName[64];

	ID3D11Buffer*		GetBuffer() { return m_pBuffer; }
	char*				GetName()	{ return m_cName; }
	
	HRESULT				Initialize();
public:
	D3D11ConstantBuffer( void ); 
	D3D11ConstantBuffer( char* a_Name ); 
	D3D11ConstantBuffer( D3D11ConstantBuffer *const );
	~D3D11ConstantBuffer( void );
};

#endif