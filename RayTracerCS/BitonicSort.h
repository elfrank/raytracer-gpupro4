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

#ifndef __BITONIC_SORT_H__
#define __BITONIC_SORT_H__

#include "D3DComputeShader.h"
#include "D3DResource.h"
#include "Geometry.h"

struct cbBitonicSort
{
    unsigned int iLevel;
    unsigned int iLevelMask;
    unsigned int iWidth;
    unsigned int iHeight;
	unsigned int viPadding[4];

	void Update( int* pData )
	{
		iLevel = pData[0];
		iLevelMask = pData[1];
		iWidth = pData[2];
		iHeight = pData[3];
		viPadding[0]=viPadding[1]=viPadding[2]=viPadding[3]=0;
	}
};

class BitonicSort
{
private:
	unsigned int		m_uiNumElements;
	unsigned int		m_uiBlockSize;
	unsigned int		m_uiTransposeBlockSize;
	unsigned int		m_uiMatrixWidth;
	unsigned int		m_uiMatrixHeight;

	D3DComputeShader*	m_csBitonicSort;
	D3DComputeShader*	m_csTranspose;
	D3DResource*		m_cbBitonicSort;

	D3DResource*		m_usNumbersToOrder1;
	D3DResource*		m_usNumbersToOrder2;
public:
	BitonicSort( ID3D11Device* pDevice );
	~BitonicSort(void);

	unsigned int		GetNumElements() { return m_uiNumElements; }
	unsigned int		GetBlockSize() { return m_uiBlockSize; }
	unsigned int		GetTransposeBlockSize() { return m_uiTransposeBlockSize; }
	unsigned int		GetMatrixWidth() { return m_uiMatrixWidth; }
	unsigned int		GetMatrixHeight() { return m_uiMatrixHeight; }

	D3DComputeShader*	GetCSBitonicSort() { return m_csBitonicSort; }
	D3DComputeShader*	GetCSTranspose() { return m_csTranspose; }
	D3DResource*		GetCBBitonicSort()  { return m_cbBitonicSort; }
	D3DResource*		GetResNumbersToOrder1()  { return m_usNumbersToOrder1; }
	D3DResource*		GetResNumbersToOrder2()  { return m_usNumbersToOrder2; }

	void GPUSort( ID3D11DeviceContext* pContext );
};

#endif