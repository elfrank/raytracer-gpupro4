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

#ifndef __D3DCOMPUTESHADER_H__
#define __D3DCOMPUTESHADER_H__

#include <Common/Common.h>

#include <D3DX11tex.h>
#include <D3DX11.h>
#include <D3DX11core.h>
#include <D3DX11async.h>
#include <D3Dcompiler.h>

// Helper class to manage the compute shaders
class D3DComputeShader
{
public:
	D3DComputeShader(void);
	D3DComputeShader(UINT x, UINT y, UINT z);
	~D3DComputeShader(void);

	ID3D11ComputeShader*				GetShader() { return m_pShader; }
	std::vector<std::pair<string, int>>	GetMacros() { return m_Macros; }
	void								SetMacroParam(unsigned int uiIndex, int iValue) { m_Macros [uiIndex].second = iValue; }
	void								AddToMacro( std::string sKey, int iValue ) { m_Macros.push_back(std::pair<std::string,int>(sKey,iValue)); }
	
	HRESULT								Load( WCHAR* pSrcFile, LPCSTR pFunctionName, 
												ID3D11Device* pDevice, 
												std::vector<std::pair<string, int>> a_Macros );
	void								Dispatch( ID3D11DeviceContext* pContext );
	void								Release() { SAFE_RELEASE( m_pShader ); }
	void								SetDimensiones( UINT x, UINT y, UINT z );
	vector<int>							GetDimensiones();
private:
	ID3D11ComputeShader*				m_pShader; // pointer to the shader to be executed
	std::vector<std::pair<string, int>> m_Macros; // its content is sent as macros on the hlsl file
	UINT								m_X; // x-dimension on the group/block execution
	UINT								m_Y; // y-dimension on the group/block execution
	UINT								m_Z; // z-dimension on the group/block execution
};

#endif