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

#include "Common.h"
#include "D3DResource.h"

D3DResource::D3DResource(void)
{
	m_pResource = NULL;
	//m_pSRV = NULL;
	//m_pUAV = NULL;

	m_pUAV = NULL;
	m_pSRV = NULL;

	m_pUAV = new D3DView<ID3D11UnorderedAccessView>();
	m_pSRV = new D3DView<ID3D11ShaderResourceView>();
}

D3DResource::~D3DResource(void)
{
	SAFE_RELEASE( m_pResource );
	//SAFE_RELEASE( m_pSRV );
	//SAFE_RELEASE( m_pUAV );

	SAFE_DELETE( m_pUAV );
	SAFE_DELETE( m_pSRV );

	//if(m_pViews.)
	//m_pViews.clear();
}

/*------------------------------------------------------------------------------------------------------
This function updates a constant buffer by simulating herency on the structures defined
at ConstantBuffers.h
------------------------------------------------------------------------------------------------------*/
/*template <class R, class T>  
HRESULT D3DResource::UpdateCB( T* pObj, D3D11Object* pDXObject )
{
	HRESULT hr;

	D3D11_MAPPED_SUBRESOURCE mp; 
	hr = pDXObject->GetDeviceContext()->Map( m_pResource,0,D3D11_MAP_WRITE_DISCARD,0,&mp );
	if( FAILED( hr ) )
	{
		printf("FAILED updating buffer.\n");
		return hr;
	}
	R* tmp = (R*)mp.pData;
	tmp->Update( pObj );
	pDXObject->GetDeviceContext()->Unmap( m_pResource,0 );

	return S_OK;
}*/

//----------------------------------------------------------------------------------------
// Init resource
//----------------------------------------------------------------------------------------
HRESULT D3DResource::Init( BufferBind a_iBind, BufferType a_iType, VOID* a_InitData, 
						  size_t a_SizeInBytes, UINT a_iNumElements, ID3D11Device* pDevice, char* pcStr )
{
	HRESULT hr = S_OK;

	m_tSize = a_SizeInBytes;
	m_eBufferType = a_iType;
	m_eBufferBind = a_iBind;
	m_tNumOfElements = a_iNumElements;

	// The implementations is an ugly if/else approach. It is clear that there are
	// better ways to do this, but for now, it works.
	if(a_iType == TEXTURE2D)
	{
		D3DTexture2D pTexture = D3DTexture2D();
		pTexture.CreateTexture2D(a_iNumElements, &m_pResource, reinterpret_cast<char*>(a_InitData), pDevice );

		if(m_pResource)
		{
			StoreName( pcStr, m_pResource );

			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory( &desc, sizeof(desc) );
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.Texture2DArray.MostDetailedMip = 0;
			desc.Texture2DArray.MipLevels = 1;
			desc.Texture2DArray.FirstArraySlice = 0;
			desc.Texture2DArray.ArraySize = a_iNumElements;
			pDevice->CreateShaderResourceView( m_pResource, &desc, m_pSRV->GetPtrView() );
		}
		else
		{
			m_pResource = NULL;
		}
	}
	else
	{
		if( a_iBind == CONSTANT)
		{
			HRESULT hr;
			D3D11_BUFFER_DESC bd;
			ZeroMemory( &bd, sizeof(D3D11_BUFFER_DESC) );
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = a_SizeInBytes;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			hr = pDevice->CreateBuffer( &bd, NULL, (ID3D11Buffer**)&m_pResource );

			StoreName( pcStr, m_pResource );
			
			if(FAILED(hr))
			{
				return hr;
			}
		}
		else
		{

			D3D11_BUFFER_DESC buffer_desc;
			ZeroMemory( &buffer_desc, sizeof(buffer_desc) );
			buffer_desc.ByteWidth = a_SizeInBytes * a_iNumElements;
			buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			buffer_desc.StructureByteStride = a_SizeInBytes;

			if(a_iBind == SRV_AND_UAV)
			{
				buffer_desc.Usage = D3D11_USAGE_DEFAULT;
				buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
			}
			else if(a_iBind == UAV)
			{
				buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			}
			else if(a_iBind == SRV)
			{
				buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			}
			else
			{
				return E_INVALIDARG;
			}

			if ( a_InitData )
			{
				D3D11_SUBRESOURCE_DATA InitData;
				InitData.pSysMem = a_InitData;
				hr = pDevice->CreateBuffer( &buffer_desc, &InitData, (ID3D11Buffer**)&m_pResource );
			} 
			else
			{
				 hr = pDevice->CreateBuffer( &buffer_desc, NULL, (ID3D11Buffer**)&m_pResource );
			}
			
			StoreName( pcStr, m_pResource );
			
			if ( FAILED( hr ) )
			{
				return hr;
			}
			
			if( a_iBind == UAV || a_iBind ==  SRV_AND_UAV)
			{
				hr = CreateUAV( pDevice );
				if (FAILED( hr ) )
					return hr;
			}
			if( a_iBind == SRV || a_iBind ==  SRV_AND_UAV )
			{
				hr = CreateSRV( pDevice );
				if (FAILED( hr ) )
					return hr;
			}
		}
	}

	Print();

	if(FAILED(hr))
	{
		printf("ERROR: Failed initializing resource.\n");
	}

	return hr;
}

//----------------------------------------------------------------------------------------
// Create UAV
//----------------------------------------------------------------------------------------
HRESULT D3DResource::CreateUAV( ID3D11Device* pDevice )
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC descBuf;
	ZeroMemory( &descBuf, sizeof(descBuf) );
	((ID3D11Buffer*)m_pResource)->GetDesc( &descBuf );

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
	{
		// This is a Raw Buffer
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = descBuf.ByteWidth / 4;
	} 
	else if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
	{
		// This is a Structured Buffer
		desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
		desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride; 
	} 
	else
	{
		return E_INVALIDARG;
	}

	hr = pDevice->CreateUnorderedAccessView( m_pResource, &desc, m_pUAV->GetPtrView() );
	
	return hr;
}

//----------------------------------------------------------------------------------------
// Create SRV
//----------------------------------------------------------------------------------------
HRESULT D3DResource::CreateSRV( ID3D11Device* pDevice )
{
	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC descBuf;
	ZeroMemory( &descBuf, sizeof(descBuf) );
	((ID3D11Buffer*)m_pResource)->GetDesc( &descBuf );

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );

	if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
	{
		// This is a Raw Buffer
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		desc.BufferEx.FirstElement = 0;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	} 
	else if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
	{
		// This is a Structured Buffer
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		desc.Buffer.FirstElement = 0;
	} 
	else
	{
		return E_INVALIDARG;
	}

	hr = pDevice->CreateShaderResourceView( m_pResource, &desc, m_pSRV->GetPtrView() );

	return hr;
}

void D3DResource::StoreName( char* pcStr, ID3D11Resource* pObject )
{
	strcpy (m_cName, pcStr);
	pObject->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( m_cName ) - 1, m_cName );
}

void D3DResource::GetName( char c_szName[] )
{
	UINT size = sizeof( m_cName ) - 1;
	m_pResource->GetPrivateData( WKPDID_D3DDebugObjectName, &size, c_szName );
}

void D3DResource::PrintName()
{
	char c_szName[256] = "";
	c_szName[0] = 0;
	GetName( c_szName );
	printf( "Name: %s\n", c_szName );
}

void D3DResource::Print()
{
	char c_szName[256] = "";
	c_szName[0] = 0;
	GetName( c_szName );

	printf( "Type: %d | Name: %s | Struct Size: %u Bytes | # Elements: %d |\n", m_eBufferType, c_szName , m_tSize, m_tNumOfElements);
}