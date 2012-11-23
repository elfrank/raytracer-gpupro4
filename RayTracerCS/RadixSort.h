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

#ifndef __RADIX_SORT_H__
#define __RADIX_SORT_H__

#include "D3DComputeShader.h"
#include "D3DResource.h"

struct RadixDebug
{
	int iValue;
	int iPastValue;
	int f;
	int b;
	int e; // !b
	int d; // final position
	int t;
	int iTotalFalses;

	void PrintString()
	{
		printf("%d | Past=%d b=%d e=%d f=%d d=%d t=%d\n",iValue,iPastValue,b,e,f,d,t);
	}
};

struct cbRadixSort
{
	int iCurrentBit;			// 4 bytes
	int iNumElements;			// 4 bytes
	float g_vfPadding[6];		// 24 bytes

	cbRadixSort()
	{
		iCurrentBit = 0;
		iNumElements = 0;
	}

	void Update( int* pData )
	{
		iCurrentBit = pData[0];
		iNumElements = pData[1];
		g_vfPadding[0]=
		g_vfPadding[1]=
		g_vfPadding[2]=
		g_vfPadding[3]=
		g_vfPadding[4]=
		g_vfPadding[5]=0.f;
	}
};

class RadixSort
{
private:
	D3DComputeShader*	m_csRadixSort;
	D3DResource*		m_cbRadixSort;
public:
	RadixSort( ID3D11Device* pDevice );
	~RadixSort(void);

	void				Sort();

	D3DComputeShader*	GetCSRadixSort() { return m_csRadixSort; }
	D3DResource*		GetCBRadixSort() { return m_cbRadixSort; }
};

#endif