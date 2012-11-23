/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2011 Advanced Micro Devices, Inc.  http://bulletphysics.org

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

// --------------------------------------------------------------------	//
// This code was modified by the authors of the framework. The original	//
// code is available at http://code.google.com/p/bullet/downloads/list. //
// --------------------------------------------------------------------	//

#include <D3DX11tex.h>
#include <D3DX11.h>
#include <D3DX11core.h>
#include <D3DX11async.h>
#include <D3Dcompiler.h>
#include <DXGI.h>

#include <sys/stat.h>
#include <vector>
#include <cmath>

namespace raytracer
{

class DeviceDX11 : public Device
{
public:
	typedef DeviceUtils::Config Config;

	ID3D11DeviceContext* m_pContext;
	ID3D11Device* m_pDevice;

	KernelManager* m_pKernelManager;

	__inline DeviceDX11() : Device( TYPE_DX11 ), m_pKernelManager(0) {}
	__inline ~DeviceDX11();
	__inline void* GetDevice() const { return m_pDevice; }
	__inline void* GetContext() const { return m_pContext; }
	__inline void Initialize( const Config& cfg );
	__inline void Release();
	template<typename T> 
	__inline void Allocate( Buffer<T>* pBuffer, int nElems, BufferBase::BufferType eType );
	template<typename T>
	__inline void Deallocate( Buffer<T>* pBuffer );
	template<typename T>
	__inline void Copy( Buffer<T>* pDest, const Buffer<T>* pSrc, int nElems );//
	template<typename T>
	__inline void Copy( T* pDest, const Buffer<T>* pSrc, int nElems, int srcOffsetNElems = 0 );//
	template<typename T>
	__inline void Copy( Buffer<T>* pDest, const T* pSrc, int nElems, int srcOffsetNElems = 0 );
	__inline void WaitForCompletion() const;
	__inline static int GetNDevices();
	__inline Kernel*	GetKernel( const char* pFileName, const char* pFunctionName, 
								const char* pOption = NULL, const char* pSrc = NULL, 
								bool bCacheKernel = true ) const;

	__inline HRESULT CreateEnvironmentMap( LPCSTR a_FileName, ID3D11ShaderResourceView** a_pSRV );
	__inline HRESULT CreateTextureInArray( LPCSTR sFileName, ID3D11Resource* ppResource, ID3D11Resource* pDest, int iArraySlice );
	__inline HRESULT CreateRandomTexture( ID3D11Texture2D* a_pTexture2D, UINT a_iWidth, UINT a_iHeight );
	__inline ID3D11Buffer* CreateAndCopyToDebugBuf( ID3D11Buffer* pBuffer );
};

#define SAFE_RELEASE1(p)      { if(p) { (p)->Release(); (p)=NULL; } }

DeviceDX11::~DeviceDX11()
{

}

void DeviceDX11::Initialize( const Config& cfg )
{
	DeviceDX11* pDevice = this;

	HRESULT hr = S_OK;
	UINT createDeviceFlag = 0;

#ifdef _DEBUG
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL levelsWanted[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	D3D_FEATURE_LEVEL featureLevel;
	
	UINT numLevelsWanted = sizeof( levelsWanted ) / sizeof( levelsWanted[0] );

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	D3D_DRIVER_TYPE driverType;
	UINT numDriverTypes = sizeof( driverTypes ) / sizeof( driverTypes[0] );

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		driverType = driverTypes[driverTypeIndex];
		UINT createDeviceFlags = NULL;

		hr = D3D11CreateDevice( NULL, driverType, NULL, createDeviceFlags, levelsWanted, numLevelsWanted,
                                          D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pContext );

		if( SUCCEEDED( hr ) ) 
		{
			if( driverType == D3D_DRIVER_TYPE_HARDWARE ) printf("Created HW Device\n");
			if( driverType == D3D_DRIVER_TYPE_REFERENCE ) printf("Created REFERENCE Device\n");
			if( featureLevel == D3D_FEATURE_LEVEL_11_0 ) printf("Created D3D_FEATURE_LEVEL_11_0 Device\n");
			if( featureLevel == D3D_FEATURE_LEVEL_10_0 ) printf("Created D3D_FEATURE_LEVEL_10_0\n");
			if( featureLevel == D3D_FEATURE_LEVEL_10_1 ) printf("Created D3D_FEATURE_LEVEL_10_1\n");
			break;
		}
	}

	IDXGIDevice * pDXGIDevice;
	hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
      
	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);
	
	IDXGIFactory * pIDXGIFactory;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);

	UINT i = 0; 
	IDXGIAdapter * pAdapter;
	pDXGIDevice->GetAdapter(&pAdapter); 
	std::vector <IDXGIAdapter*> vAdapters; 
	while(pIDXGIFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) 
	{ 
		vAdapters.push_back(pAdapter); 
		++i; 
	} 


	//ADLASSERT( moduleD3D11 );

	m_pKernelManager = new KernelManager;
}

void DeviceDX11::Release( void ) 
{
	SAFE_RELEASE1( m_pContext );
	SAFE_RELEASE1( m_pDevice );

	if( m_pKernelManager ) delete m_pKernelManager;
}

template<typename T>
void DeviceDX11::Allocate( Buffer<T>* pBuffer, int nElems, BufferBase::BufferType eType )
{
	ASSERT( eType != BufferBase::BUFFER_ZERO_COPY );

	DeviceDX11* pDeviceData = this;
	pBuffer->m_pDevice = pDeviceData;
	pBuffer->m_tSize = nElems;
	BufferDX11<T>* dBuffer = (BufferDX11<T>*)pBuffer;

	HRESULT hr = S_OK;

	if( eType == BufferBase::BUFFER_CONST )
	{
		ASSERT( nElems == 1 );
		D3D11_BUFFER_DESC constant_buffer_desc;
		ZeroMemory( &constant_buffer_desc, sizeof(constant_buffer_desc) );
		constant_buffer_desc.ByteWidth = (((sizeof(T))/(16) + (((sizeof(T))%(16)==0)?0:1))*(16));
		constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC; //D3D11_USAGE_DEFAULT; //D3D11_USAGE_DYNAMIC
		constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//0; //D3D11_CPU_ACCESS_WRITE

		hr = pDeviceData->m_pDevice->CreateBuffer( &constant_buffer_desc, NULL, dBuffer->GetBufferPtr() );
		ASSERT( hr == S_OK );
		return;
	}

	D3D11_BUFFER_DESC buffer_desc;
	ZeroMemory( &buffer_desc, sizeof( buffer_desc ) );
	buffer_desc.ByteWidth = nElems * sizeof(T);
	buffer_desc.StructureByteStride = sizeof(T);

	if( eType != BufferBase::BUFFER_RAW )
	{
		buffer_desc.StructureByteStride = sizeof( T );
	}

	if( eType == BufferBase::BUFFER_STAGING )
	{
		buffer_desc.Usage = D3D11_USAGE_STAGING;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	}
	else if( eType == BufferBase::BUFFER_INDEX )
	{
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	}
	else if( eType == BufferBase::BUFFER_VERTEX )
	{
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	}
	else
	{
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		if( eType == BufferBase::BUFFER_RAW )
		{
			buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS | D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS; // need this to be used for DispatchIndirect
		}
	}

	hr = pDeviceData->m_pDevice->CreateBuffer( &buffer_desc, NULL, dBuffer->GetBufferPtr() );

	ASSERT( hr == S_OK )

	if( eType == BufferBase::BUFFER_INDEX ) return;
	
	if( eType == BufferBase::BUFFER ||
		eType == BufferBase::BUFFER_RAW ||
		eType == BufferBase::BUFFER_W_COUNTER )
	{
		// Create UAVs for all CS buffers
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavBuffer_desc;
		ZeroMemory( &uavBuffer_desc, sizeof(uavBuffer_desc) );
		uavBuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		
		if( eType == BufferBase::BUFFER_RAW )
		{
			uavBuffer_desc.Format = DXGI_FORMAT_R32_TYPELESS;
			uavBuffer_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
			uavBuffer_desc.Buffer.NumElements = buffer_desc.ByteWidth / 4;
		}
		else
		{
			uavBuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
			uavBuffer_desc.Buffer.NumElements = nElems;
		}

		if( eType == BufferBase::BUFFER_W_COUNTER )
		{
			uavBuffer_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
		}

		hr = pDeviceData->m_pDevice->CreateUnorderedAccessView( dBuffer->GetBuffer(), &uavBuffer_desc, dBuffer->GetUAVPtr() );
		ASSERT( hr == S_OK );

		// Create SRVs for all CS buffers
		D3D11_SHADER_RESOURCE_VIEW_DESC srvBuffer_desc;
		ZeroMemory( &srvBuffer_desc, sizeof( srvBuffer_desc ) );
		if( eType == BufferBase::BUFFER_RAW )
		{
			ASSERT( sizeof(T) <= 16 )
			{
				srvBuffer_desc.Format = DXGI_FORMAT_R32_UINT;
				srvBuffer_desc.Buffer.ElementWidth = nElems;
			}
		}
		else
		{
			srvBuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
			srvBuffer_desc.Buffer.ElementWidth = nElems;
		}
		srvBuffer_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

		hr = pDeviceData->m_pDevice->CreateShaderResourceView( dBuffer->GetBuffer(), &srvBuffer_desc, dBuffer->GetSRVPtr() );
		ASSERT( hr == S_OK );
	}
	else if( eType == BufferBase::BUFFER_APPEND )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;

		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Buffer.NumElements = buffer_desc.ByteWidth / buffer_desc.StructureByteStride;
		
		hr = pDeviceData->m_pDevice->CreateUnorderedAccessView( dBuffer->GetBuffer(), &desc, dBuffer->GetUAVPtr() );
		ASSERT( hr == S_OK )
	}
}

template<typename T>
void DeviceDX11::Deallocate( Buffer<T>* pBuffer )
{
	BufferDX11<T>* dBuffer = (BufferDX11<T>*)pBuffer;
	
	if( dBuffer->GetBuffer() )
	{
		dBuffer->GetBuffer()->Release();
		dBuffer->m_pPtr = NULL;
	}
	if( dBuffer->GetUAV() )
	{
		dBuffer->GetUAV()->Release();
		dBuffer->m_pUAV = NULL;
	}
	if( dBuffer->GetSRV() )
	{
		dBuffer->GetSRV()->Release();
		dBuffer->m_pSRV = NULL;
	}
	pBuffer->m_pDevice = 0;
}

void DeviceDX11::WaitForCompletion() const 
{
	const DeviceDX11* pDeviceData = this;

	ID3D11Query* syncQuery;
	D3D11_QUERY_DESC qDesc;
	qDesc.Query = D3D11_QUERY_EVENT;
	qDesc.MiscFlags = 0;
	pDeviceData->m_pDevice->CreateQuery( &qDesc, &syncQuery );
	pDeviceData->m_pContext->End( syncQuery );
	while ( pDeviceData->m_pContext->GetData( syncQuery, 0, 0, 0 ) == S_FALSE ) {}
	syncQuery->Release();
}

Kernel*	DeviceDX11::GetKernel( const char* pFileName, const char* pFunctionName, 
								const char* pOption, const char* pSrc, 
								bool bCacheKernel ) const 
{
	return m_pKernelManager->Query( this, pFileName, pFunctionName, pOption, pSrc, bCacheKernel );
}

template<typename T>
void DeviceDX11::Copy( Buffer<T>* pDst, const Buffer<T>* pSrc, int nElems )
{
	if( (pDest->m_pDevice->m_eType == TYPE_DX11) || (pSrc->m_pDevice->m_eType == TYPE_DX11) )
	{
		DeviceDX11* pDeviceData = this;
		BufferDX11<T>* dDst = (BufferDX11<T>*)pDst;
		BufferDX11<T>* dSrc = (BufferDX11<T>*)pSrc;

		D3D11_MAPPED_SUBRESOURCE MappedVelResource = {0};

		D3D11_BOX destRegion;
		destRegion.left = 0*sizeof(T);
		destregion.front = 0;
		destRegion.top = 0;
		destRegion.bottom = 1;
		destRegion.back = 1;
		destRegion.right = ( 0+nElems )*sizeof(T);

		pDeviceData->m_pContext->CopySubresourceRegion( 
			dDst->GetBuffer(),
			0, 0, 0, 0,
			dSrc->GetBuffer(),
			0,
			&destRegion );
	}
	else if( pSrc->m_pDevice->m_eType == TYPE_HOST )
	{
		ASSERT( pDst->GetType() == TYPE_DX11 );
		pDst->Write( pSrc->m_pPtr, nElems );
	}
	else if( pDst->m_pDevice->m_eType == TYPE_HOST )
	{
		ASSERT( pSrc->GetType() == TYPE_DX11 );
		pSrc->Read( pDst->m_pPtr, nElems );
	}
	else
	{
		ASSERT( 0 );
	}
}

template<typename T>
void DeviceDX11::Copy( T* pDst, const Buffer<T>* pSrc, int nElems, int srcOffsetNElems )
{
	DeviceDX11* pDeviceData = this;
	BufferDX11<T>* dSrc = (BufferDX11<T>*)pSrc;
	Buffer<T> sBuf( pDeviceData, nElems, BufferBase::BUFFER_STAGGING );
	BufferDX<T>* dStagingBuf = (BufferDX11<T>*)&sBuf;

	ID3D11Buffer* StagingBuffer = dStagingbuf->GetBuffer();
	D3D11_MAPPED_SUBRESOURCE MappedVelResource = {0};

	D3D11_BOX destRegion;
	destRegion.left = srcOffsetNElems*sizeof(T);
	destRegion.front = 0;
	destRegion.top = 0;
	destRegion.bottom = 1;
	destRegion.back = 1;
	destRegion.right = (srcOffsetNElems+nElems)*sizeof(T);

	pDeviceData->m_pContext->Map( StagingBuffer, 0, D3D11_MAP_READ, 0, &MappedVelResource );
	memcpy( pDst, MappedVelResource.pData, nElems*sizeof(T) );
	pDeviceData->m_pContext->Unmap( StagingBuffer, 0 );
}


template<typename T>
void DeviceDX11::Copy( Buffer<T>* pDst, const T* pSrc, int nElems, int dstOffsetNElems )
{
	BufferDX11<T>* dBuf = (BufferDX11<T>*)pDst;

	DeviceDX11* pDeviceData = this;

	D3D11_BOX destRegion;
	destRegion.left = dstOffsetNElems*sizeof(T);
	destRegion.front = 0;
	destRegion.top = 0;
	destRegion.bottom = 1;
	destRegion.back = 1;
	destRegion.right = (dstOffsetNElems+nElems)*sizeof(T);
	pDeviceData->m_pContext->UpdateSubresource( dBuf->GetBuffer(), 0, &destRegion, pSrc, 0, 0 );
}

HRESULT	DeviceDX11::CreateEnvironmentMap( LPCSTR sFileName, ID3D11ShaderResourceView** ppSRV )
{
	HRESULT hr = S_OK;

    ID3D11Resource* pRes = NULL;
    ID3D11Texture2D* pCubeTexture = NULL;
    ID3D11ShaderResourceView* pCubeRV = NULL;

    D3DX11_IMAGE_LOAD_INFO LoadInfo;
    LoadInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3DX11CreateTextureFromFileA( m_pDevice, sFileName, &LoadInfo, NULL, &pRes, NULL );
    if( pRes )
    {
		printf("Create environment mapping %s\n", sFileName);
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_TEXTURE2D_DESC ) );
        pRes->QueryInterface( __uuidof( ID3D11Texture2D ), ( LPVOID* )&pCubeTexture );
        pCubeTexture->GetDesc( &desc );
		if(pRes) pRes->Release();
		SAFE_RELEASE1( pRes );

        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
        ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
        SRVDesc.Format = desc.Format;
        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        SRVDesc.TextureCube.MipLevels = desc.MipLevels;
        SRVDesc.TextureCube.MostDetailedMip = 0;
		hr = m_pDevice->CreateShaderResourceView( pCubeTexture, &SRVDesc, ppSRV );
    }

	if(FAILED(hr))
	{
		printf("FAILED to create Environment Map.\n");
	}

	return hr;
}

HRESULT DeviceDX11::CreateTextureInArray( LPCSTR sFileName, ID3D11Resource* ppResource, ID3D11Resource* pDest, int iArraySlice )
{
	HRESULT hr = S_OK;

	struct stat stFileInfo; 
	if( !stat(sFileName,&stFileInfo) )
	{
		printf("Create texture %s\n", sFileName);
		m_pContext->CopySubresourceRegion(pDest, D3D11CalcSubresource(0, iArraySlice, 1), 0, 0, 0, ppResource, 0, NULL);
	}

	return hr;
}

HRESULT DeviceDX11::CreateRandomTexture( ID3D11Texture2D* pTexture2D, UINT iWidth, UINT iHeight )
{
	HRESULT hr = S_OK;

	//update random texture
	D3D11_MAPPED_SUBRESOURCE mappedTex;
	m_pContext->Map(pTexture2D,0,D3D11_MAP_WRITE_DISCARD ,0, &mappedTex );

	for( UINT row = 0; row < iHeight; row++ )
	{
		float* pTexels = (float*)((char*)mappedTex.pData+row * mappedTex.RowPitch);
		for( UINT col = 0; col < iWidth; col++ )
		{
			UINT colStart = col * 4;
			float length;
			do
			{
				pTexels[colStart + 0] = 1.0f-2.0f*float(rand())/float(RAND_MAX);// Red
				pTexels[colStart + 1] = 1.0f-2.0f*float(rand())/float(RAND_MAX);
				pTexels[colStart + 2] = 1.0f-2.0f*float(rand())/float(RAND_MAX);
				pTexels[colStart + 3] = float(rand())/float(RAND_MAX);
				length =	(pTexels[colStart + 0]*pTexels[colStart + 0])+
							(pTexels[colStart + 1]*pTexels[colStart + 1])+
							(pTexels[colStart + 2]*pTexels[colStart + 2]);
			}while(length>1.0f);
			length=sqrt(length);
			pTexels[colStart + 0]/=length;
			pTexels[colStart + 1]/=length;
			pTexels[colStart + 2]/=length;
		}
	}

	m_pContext->Unmap(pTexture2D,0);
	return hr;
}

//--------------------------------------------------------------------------------------
// Create a CPU accessible buffer and download the content of a GPU buffer into it
// This function is very useful for debugging CS programs
//-------------------------------------------------------------------------------------- 
ID3D11Buffer* DeviceDX11::CreateAndCopyToDebugBuf( ID3D11Buffer* pBuffer )
{
    ID3D11Buffer* debugbuf = NULL;

    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof(desc) );
    pBuffer->GetDesc( &desc );
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;
    if ( SUCCEEDED(m_pDevice->CreateBuffer(&desc, NULL, &debugbuf)) )
    {
		m_pContext->CopyResource( debugbuf, pBuffer );
    }

    return debugbuf;
}
/*
//--------------------------------------------------------------------------------------
// Helper function to compile an hlsl shader from file, 
// its binary compiled code is returned
//--------------------------------------------------------------------------------------
HRESULT D3D11Object::CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, 
LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    // find the file
    //WCHAR str[MAX_PATH];
    //V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, szFileName ) );
	WCHAR *str = szFileName;
    // open the file 
	//"createFile" creates or opens the file
    HANDLE hFile = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, 
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN, NULL );
    if( INVALID_HANDLE_VALUE == hFile )
        return E_FAIL;

    // Get the file size
    LARGE_INTEGER FileSize;
    GetFileSizeEx( hFile, &FileSize );

    // create enough space for the file data
    BYTE* pFileData = new BYTE[ FileSize.LowPart ];
    if( !pFileData )
        return E_OUTOFMEMORY;

    // read the data in
    DWORD BytesRead;
    if( !ReadFile( hFile, pFileData, FileSize.LowPart, &BytesRead, NULL ) )
        return E_FAIL; 

    CloseHandle( hFile );

    // Compile the shader
    char pFilePathName[MAX_PATH];        
    WideCharToMultiByte(CP_ACP, 0, str, -1, pFilePathName, MAX_PATH, NULL, NULL);
    ID3DBlob* pErrorBlob;
    hr = D3DCompile( pFileData, FileSize.LowPart, pFilePathName, NULL, NULL, szEntryPoint, 
                      szShaderModel, D3D10_SHADER_ENABLE_STRICTNESS, 0, ppBlobOut, &pErrorBlob );


    delete []pFileData;
  
    if( FAILED(hr) )
    {
        printf( (char*)pErrorBlob->GetBufferPointer() );
        SAFE_RELEASE( pErrorBlob );
        return hr;
    }
    SAFE_RELEASE( pErrorBlob );

    return S_OK;
}*/

#undef SAFE_RELEASE1

};