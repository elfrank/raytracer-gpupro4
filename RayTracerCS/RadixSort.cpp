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

#include "RadixSort.h"

RadixSort::RadixSort( ID3D11Device* pDevice )
{
	m_csRadixSort = new D3DComputeShader();
	m_cbRadixSort = new D3DResource();

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

	m_csRadixSort->Load((WCHAR*)L"./RayTracerCS/Shaders/App.hlsl",
		(LPCSTR)"CSRadixSort",pDevice, macros);
	m_csRadixSort->SetDimensiones(1024, 1, 1);
}

RadixSort::~RadixSort(void)
{
	SAFE_DELETE( m_csRadixSort );
	SAFE_DELETE( m_cbRadixSort );
}


void RadixSort::Sort()
{
	/*unsigned int iNumElementsPowerOfTwo = g_iNumElements;
	GetNextPowerOfTwo(iNumElementsPowerOfTwo);
	const int numberOfIterations = 31;

	float timer = UpdateTime();
	for(int i = 0; i <numberOfIterations; ++i)
	{
		int * data = new int[2];
		data[0] = i;
		data[1] = g_iNumElements;
		UpdateCB<cbRadixSort, int>(m_cbRadixSort->GetResource(), data);
		m_csRadixSort.Dispatch( m_pDXObject );

		//DebugCSBuffer<RadixDebug>(m_uNumbersToOrder.GetResource(), iNumElementsPowerOfTwo);
	}
	timer = UpdateTime()-timer;

	printf("Radix Sort Time = %f\n",timer);*/

	/*for(unsigned int i = 0; i < g_iNumElements; ++i)
	{
		if( ((1023-(i%1024))*1024) != pBuffer[i] )
		{
			printf("FAIL\n");
		}
	}*/

	//DebugCSBuffer<RadixDebug>(m_pSConstruction->GetSort()->GetResNumbersToOrder1()->GetResource(), iNumElementsPowerOfTwo);
}