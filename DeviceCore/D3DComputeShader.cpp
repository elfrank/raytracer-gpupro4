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

#include <sstream>
#include "D3DComputeShader.h"

D3DComputeShader::D3DComputeShader(void)
{
	m_pShader = NULL;
	m_X = m_Y = m_Z = 1;
}

D3DComputeShader::D3DComputeShader(UINT x, UINT y, UINT z)
{
	m_pShader = NULL;
	SetDimensiones(x,y,z);
}

D3DComputeShader::~D3DComputeShader(void)
{
	m_pShader->Release();
}

//-----------------------------------------------------------------------------------------
// Load and compile the shader
//-----------------------------------------------------------------------------------------
HRESULT D3DComputeShader::Load( WCHAR* pSrcFile, LPCSTR pFunctionName, ID3D11Device* pDevice, 
							   std::vector<std::pair<string, int>> a_Macros )
{
	m_Macros = a_Macros;

	HRESULT hr = S_OK;

	ID3DBlob* pBlob = NULL;         // used to store the compiled compute shader
	ID3DBlob* pErrorBlob = NULL;    // used to store any compilation errors

	D3D10_SHADER_MACRO Shader_Macros[10] = { "BLOCK_SIZE_X", "2", 
											"BLOCK_SIZE_Y", "1", 
											"BLOCK_SIZE_Z","1", "N", 
											"0" ,
											NULL, NULL,
											NULL, NULL,
											NULL, NULL,
											NULL, NULL,
											NULL, NULL,
											NULL, NULL,
	};

	// Use shader macros to dynamically define variables on the shader
	std::vector<string> str(m_Macros.size());
	for(unsigned int i = 0; i < m_Macros.size(); ++i)
	{		
		std::stringstream ss;
		ss << m_Macros[i].second;
		str[i] = ss.str();

		Shader_Macros[i].Name = m_Macros[i].first.data();
		Shader_Macros[i].Definition = str[i].data();	
	}

	hr = D3DX11CompileFromFile(
		pSrcFile,                   // use the code in this file
		Shader_Macros,              // use additional defines
		NULL,                       // don't use additional includes
		pFunctionName,              // compile this function
		"cs_5_0",                   // use compute shader 5.0
		NULL,                       // no compile flags
		NULL,                       // no effect flags
		NULL,                       // don't use a thread pump
		&pBlob,                     // store the compiled shader here
		&pErrorBlob,                // store any errors here
		NULL );                     // no thread pump is used, so no asynchronous HRESULT is needed

	 // if there were any errors, display them
	if( pErrorBlob )
	{
		printf("%s\n", (char*)pErrorBlob->GetBufferPointer());
	}

	if( FAILED(hr) )
		return hr;

	// if the compute shader was compiled successfully, create it on the GPU
	pDevice->CreateComputeShader(
		pBlob->GetBufferPointer(),  // use the compute shader that was compiled here
		pBlob->GetBufferSize(),     // with this size
		NULL,                       // don't use any dynamic linkage
		&m_pShader );      // store the reference to the compute shader here

	return hr;
}

//-----------------------------------------------------------------------------------------
// Execute the shader
//-----------------------------------------------------------------------------------------
void D3DComputeShader::Dispatch( ID3D11DeviceContext* pContext )
{
	pContext->CSSetShader( m_pShader, NULL, 0 );
	pContext->Dispatch( m_X, m_Y, m_Z );
}

//-----------------------------------------------------------------------------------------
// Change the number of threads to execute per group/block
//-----------------------------------------------------------------------------------------
void D3DComputeShader::SetDimensiones(UINT x, UINT y, UINT z)
{
	m_X = x;
	m_Y = y;
	m_Z = z;
}

vector<int> D3DComputeShader::GetDimensiones()
{
	vector<int> dim;
	dim.push_back(m_X);
	dim.push_back(m_Y);
	dim.push_back(m_Z);
	return dim;
}