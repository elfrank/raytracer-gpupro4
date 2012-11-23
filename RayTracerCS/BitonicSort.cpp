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

#include <Common/Common.h>
#include "BitonicSort.h"

BitonicSort::BitonicSort( ID3D11Device* pDevice )
{
	// The number of elements to sort is limited to an even power of 2
	// At minimum 8,192 elements - BITONIC_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE
	// At maximum 262,144 elements - BITONIC_BLOCK_SIZE * BITONIC_BLOCK_SIZE
	m_uiNumElements = 512*512;
	m_uiBlockSize = 512;
	m_uiTransposeBlockSize = 16;
	m_uiMatrixWidth = m_uiBlockSize;
	m_uiMatrixHeight = m_uiNumElements / m_uiBlockSize;

	m_csBitonicSort = new D3DComputeShader();
	m_csTranspose = new D3DComputeShader();
	m_cbBitonicSort = new D3DResource();

	m_usNumbersToOrder1 = new D3DResource();
	m_usNumbersToOrder2 = new D3DResource();

	m_usNumbersToOrder1->Init(SRV_AND_UAV, STRUCTURED, NULL, sizeof(MortonCode32),
		m_uiNumElements, pDevice, "usNumbersToOrder1");
	m_usNumbersToOrder2->Init(SRV_AND_UAV,STRUCTURED,NULL, sizeof(MortonCode32), 
		m_uiNumElements, pDevice, "usNumbersToOrder2");

	m_cbBitonicSort->Init( CONSTANT, STRUCTURED, NULL, sizeof(cbBitonicSort), 1, pDevice, "cbBitonicSort" );
}

BitonicSort::~BitonicSort(void)
{
	SAFE_DELETE( m_csBitonicSort );
	SAFE_DELETE( m_csTranspose );

	SAFE_DELETE( m_usNumbersToOrder1 );
	SAFE_DELETE( m_usNumbersToOrder2 );
}

//--------------------------------------------------------------------------------------
// GPU Bitonic Sort
//--------------------------------------------------------------------------------------
void BitonicSort::GPUSort( ID3D11DeviceContext* pContext )
{
	int cbData [] = { 0,0,0,0 };

	m_usNumbersToOrder2->GetUAV()->SetId( m_usNumbersToOrder1->GetUAV()->GetId() );
	m_usNumbersToOrder1->GetSRV()->SetId( m_usNumbersToOrder2->GetSRV()->GetId() );

    // Sort the data
    // First sort the rows for the levels <= to the block size
    for( unsigned int level = 2 ; level <= GetBlockSize() ; level = level * 2 )
    {
		//SetConstants( level, level, m_pBitonicSort-GetMatrixHeight(), GetMatrixWidth() );
		cbData[0] = level; /* level */
		cbData[1] = level; /* level mask */
		cbData[2] = GetMatrixHeight(); /* width */
		cbData[3] = GetMatrixWidth(); /* height */
		m_cbBitonicSort->UpdateCB<cbBitonicSort, int>( cbData, pContext );

        // Sort the row data
		pContext->CSSetUnorderedAccessViews( m_usNumbersToOrder1->GetUAV()->GetId(), 1, m_usNumbersToOrder1->GetUAV()->GetPtrView(), NULL );
		m_csBitonicSort->SetDimensiones( GetNumElements() / GetBlockSize(), 1, 1 );
		m_csBitonicSort->Dispatch( pContext );
    }
    
    // Then sort the rows and columns for the levels > than the block size
    // Transpose. Sort the Columns. Transpose. Sort the Rows.
    for( unsigned int level = (GetBlockSize() * 2) ; level <= GetNumElements() ; level = level * 2 )
    {
        //SetConstants( (level / m_pBitonicSort-GetBlockSize()), (level & ~GetNumElements()) / m_pBitonicSort-GetBlockSize(), GetMatrixWidth(), m_pBitonicSort-GetMatrixHeight() );
		cbData[0] = (level / GetBlockSize()); /* level */
		cbData[1] = (level & ~GetNumElements()) / GetBlockSize(); /* level mask */
		cbData[2] = GetMatrixWidth(); /* width */
		cbData[3] = GetMatrixHeight(); /* height */
		m_cbBitonicSort->UpdateCB<cbBitonicSort, int>( cbData, pContext );

        // Transpose the data from buffer 1 into buffer 2
		m_usNumbersToOrder2->SetUAVs( pContext );
		m_usNumbersToOrder1->SetShaderResources( pContext );
		m_csTranspose->SetDimensiones(GetMatrixWidth() / GetTransposeBlockSize(), GetMatrixHeight() / GetTransposeBlockSize(), 1);
		m_csTranspose->Dispatch( pContext );

        // Sort the transposed column data
		m_csBitonicSort->SetDimensiones( GetNumElements() / GetBlockSize(), 1, 1 );
		m_csBitonicSort->Dispatch( pContext );

		cbData[0] = GetBlockSize(); /* level */
		cbData[1] = level; /* level mask */
		cbData[2] = GetMatrixHeight(); /* width */
		cbData[3] = GetMatrixWidth(); /* height */
		m_cbBitonicSort->UpdateCB<cbBitonicSort, int>( cbData, pContext );

        // Transpose the data from buffer 2 back into buffer 1
		m_usNumbersToOrder1->SetUAVs( pContext );
		m_usNumbersToOrder2->SetShaderResources( pContext );
		m_csTranspose->SetDimensiones( GetMatrixHeight() / GetTransposeBlockSize(), GetMatrixWidth() / GetTransposeBlockSize(), 1);
		m_csTranspose->Dispatch( pContext );

        // Sort the row data
        m_csBitonicSort->SetDimensiones( GetNumElements() / GetBlockSize(), 1, 1 );
		m_csBitonicSort->Dispatch( pContext );
    }
}