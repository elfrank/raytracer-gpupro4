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

#ifndef __STRUCTURE_CONSTRUCTION_H__
#define __STRUCTURE_CONSTRUCTION_H__

#include "D3DComputeShader.h"
#include "D3DResource.h"

#include "BitonicSort.h"

struct cbSLBVH
{
	unsigned int uiLevel;
	unsigned int uiDepth;
	unsigned int viPadding[6];

	cbSLBVH()
	{
		uiLevel = 1;
		uiDepth = 18;
	}

	void Update( int* pData )
	{
		uiLevel = pData[0];
		uiDepth = pData[1];
	}
};

struct cbBBVH
{
	unsigned int g_uiCurrentLevel;
	unsigned int g_uiBlockSize_x;
	unsigned int g_uiBlockSize_y;
	unsigned int g_uiBlockSize_z;
	unsigned int viPadding[4];

	cbBBVH()
	{
		g_uiCurrentLevel = 0;
		g_uiBlockSize_x = 1;
		g_uiBlockSize_y = 1;
		g_uiBlockSize_z = 1;
	}

	void Update( int* pData )
	{
		g_uiCurrentLevel = pData[0];
		g_uiBlockSize_x = pData[1];
		g_uiBlockSize_y = pData[2];
		g_uiBlockSize_z = pData[3];
	}
};

class StructureConstruction
{
private:
	D3DComputeShader*						m_csPreBuildStructure;
	D3DComputeShader*						m_csBuildStructure;
	D3DComputeShader*						m_csPosBuildStructure;
	D3DComputeShader*						m_csBuildBoxes;
	D3DResource*							m_cbSLBVH;
	D3DResource*							m_cbBBVH;
	BitonicSort*							m_pBitonicSort;
public:
	StructureConstruction( ID3D11Device* pDevice );
	~StructureConstruction(void);

	void						GPUBuildStructure( unsigned int uiDepth, ID3D11Device* pDevice, ID3D11DeviceContext* pContext );
	void						GPUBuildBBVH( unsigned int uiDepth, ID3D11DeviceContext* pContext );

	D3DComputeShader*			GetCSPreBuildStructure() { return m_csPreBuildStructure; }
	D3DComputeShader*			GetCSBuildStructure() { return m_csBuildStructure; }
	D3DComputeShader*			GetCSPosBuildStructure() { return m_csPosBuildStructure; }
	D3DComputeShader*			GetCSBuildBoxes() { return m_csBuildBoxes; }
	D3DResource*				GetCBStructure() { return m_cbSLBVH; }
	D3DResource*				GetCBBBVH() { return m_cbBBVH; }
	BitonicSort*				GetSort() { return m_pBitonicSort; }
};

#endif