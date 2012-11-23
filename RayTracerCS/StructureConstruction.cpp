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

#include "StructureConstruction.h"

StructureConstruction::StructureConstruction( ID3D11Device* pDevice )
{
	m_csPreBuildStructure = new D3DComputeShader();
	m_csBuildStructure = new D3DComputeShader();
	m_csPosBuildStructure = new D3DComputeShader();
	m_csBuildBoxes = new D3DComputeShader();
	m_cbSLBVH = new D3DResource();
	m_cbBBVH = new D3DResource();
	m_pBitonicSort = new BitonicSort( pDevice );

	// Compile and create compute shader
	unsigned int N = 0;
	std::vector<std::pair<string, int>> macros(4);
	macros[0] = std::pair<string,int>("BLOCK_SIZE_X",0);
	macros[1] = std::pair<string,int>("BLOCK_SIZE_Y",0);
	macros[2] = std::pair<string,int>("BLOCK_SIZE_Z",0);
	macros[3] = std::pair<string,int>("N",N);

	macros[0].second = 512;
	macros[1].second = 1;
	macros[2].second = 1;

	m_pBitonicSort->GetCSBitonicSort()->Load(
		(WCHAR*)L"./RayTracerCS/Shaders/App.hlsl",(LPCSTR)"CSBitonicSort",
		pDevice, macros);

	m_pBitonicSort->GetCSTranspose()->Load(
		(WCHAR*)L"./RayTracerCS/Shaders/App.hlsl",(LPCSTR)"CSMatrixTranspose",
		pDevice, macros);

	m_csPreBuildStructure->Load(
		(WCHAR*)L"./RayTracerCS/Shaders/App.hlsl",(LPCSTR)"CSPreBuildStructure",
		pDevice, macros);
	m_csPreBuildStructure->SetDimensiones(
		m_pBitonicSort->GetNumElements()/
		m_pBitonicSort->GetBlockSize(), 1, 1);

	m_csBuildStructure->Load(
		(WCHAR*)L"./RayTracerCS/Shaders/App.hlsl",(LPCSTR)"CSBuildStructure",
		pDevice, macros);
	m_csBuildStructure->SetDimensiones(512, 1, 1);

	m_csPosBuildStructure->Load(
		(WCHAR*)L"./RayTracerCS/Shaders/App.hlsl",(LPCSTR)"CSPosBuildStructure",
		pDevice, macros);
	m_csPosBuildStructure->SetDimensiones(512, 1, 1);

	//unsigned int numThreads = 32;
	//GetNextPowerOfTwo(numThreads);
	//macros[0].second = numThreads;

	m_csBuildBoxes->Load(
		(WCHAR*)L"./RayTracerCS/Shaders/App.hlsl",(LPCSTR)"CSReduce",
		pDevice, macros);
	
	m_csBuildBoxes->SetDimensiones(512, 1,1);
}

StructureConstruction::~StructureConstruction(void)
{
	SAFE_DELETE( m_csPreBuildStructure );
	SAFE_DELETE( m_csBuildStructure );
	SAFE_DELETE( m_csPosBuildStructure );
	SAFE_DELETE( m_csBuildBoxes );
	SAFE_DELETE( m_cbSLBVH );
	SAFE_DELETE( m_cbBBVH );
	SAFE_DELETE( m_pBitonicSort );
}

void StructureConstruction::GPUBuildStructure( unsigned int uiDepth, ID3D11Device* pDevice, ID3D11DeviceContext* pContext )
{
	/* compute Morton Codes for each primitive */
	m_csPreBuildStructure->Dispatch( pContext );
	/* sort (bitonic sort) primitives using its morton code as key */
	m_pBitonicSort->GPUSort( pContext );

	/* create temporaty buffer for SLBVH CB */
	int LBVHData[] = { 1, uiDepth };

	/* create query for synchronous dispatches */
	D3D11_QUERY_DESC pQueryDesc;
	pQueryDesc.Query = D3D11_QUERY_EVENT;
	pQueryDesc.MiscFlags = 0;
	ID3D11Query *pEventQuery;
	pDevice->CreateQuery( &pQueryDesc, &pEventQuery );

	/* build each level of the SLBVH (top-down) */
	for(unsigned int i = 1; i < uiDepth; i++)
	{
		/* set current level of the tree */
		LBVHData[0] = i;
		m_cbSLBVH->UpdateCB<cbSLBVH,int>( LBVHData, pContext );

		/* dispatch shader */
		m_csBuildStructure->SetDimensiones(1024,1,1);
		m_csBuildStructure->Dispatch( pContext );

		/* wait until all nodes in a level have been created (avoiding race conditions)*/
		pContext->End(pEventQuery);
	}
	/* release query */
	pEventQuery->Release();

	/* compute boxes for each level of the SLBVH (down-top) */
	for( int i = (uiDepth-1); i >= 0; i-- )
	{
		/* set current level of the tree */
		LBVHData[0] = i;
		m_cbSLBVH->UpdateCB<cbSLBVH,int>( LBVHData, pContext );

		/* dispatch shader */
		m_csPosBuildStructure->SetDimensiones(1024,1,1);
		m_csPosBuildStructure->Dispatch( pContext );
	}
}

void StructureConstruction::GPUBuildBBVH( unsigned int uiNumNodes, ID3D11DeviceContext* pContext )
{
	m_csPreBuildStructure->Dispatch( pContext );

	unsigned int dim = 32;
	GetNextPowerOfTwo(dim);

	int BBVHData[] = { 1, 1, 1, 1 };
	m_csPosBuildStructure->SetDimensiones(dim,1,1);

	for(unsigned int i = uiNumNodes; i>0; i >>= 1)
	{
		//BBVHData[0] = level;
		m_csPosBuildStructure->SetDimensiones(512,1,1);
		BBVHData[1] = m_csPreBuildStructure->GetDimensiones()[0];
		m_cbBBVH->UpdateCB<cbBBVH,int>( BBVHData, pContext );

		//dim = uiNumNodes>>(1+level);
		//dim>>=1;
		//m_csPosBuildStructure->SetDimensiones(min(1024,dim),1,1);
		
		m_csBuildBoxes->Dispatch( pContext );
	}
	

}