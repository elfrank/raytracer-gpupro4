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

#include "D3DConstantBuffer.h"

D3D11ConstantBuffer::D3D11ConstantBuffer(void)
{
}

D3D11ConstantBuffer::~D3D11ConstantBuffer(void)
{
	m_pBuffer->Release();
}

D3D11ConstantBuffer::D3D11ConstantBuffer( D3D11ConstantBuffer *const a_Buffer )
{

}