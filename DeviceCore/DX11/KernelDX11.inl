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

namespace raytracer
{

#define SAFE_RELEASE1(p)      { if(p) { (p)->Release(); (p)=NULL; } }

struct KernelDX11 : public Kernel
{
	ID3D11ComputeShader* GetKernel() { return (ID3D11ComputeShader*)m_pKernel; }
	ID3D11ComputeShader** GetKernelPtr() { return (ID3D11ComputeShader**)&m_pKernel; }
};

__inline
#ifdef UNICODE
HRESULT FindDXSDKShaderFileCch( __in_ecount(cchDest) WCHAR* strDestPath,
                                int cchDest, 
                                __in LPCWSTR strFilename )
#else
HRESULT FindDXSDKShaderFileCch( __in_ecount(cchDest) CHAR* strDestPath,
                                int cchDest, 
                                __in LPCSTR strFilename )

#endif
{

	if( NULL == strFilename || strFilename[0] == 0 || NULL == strDestPath || cchDest < 10 )
        return E_INVALIDARG;

	 // Get the exe name, and exe path
#ifdef UNICODE
    WCHAR strExePath[MAX_PATH] =
#else
    CHAR strExePath[MAX_PATH] =
#endif
    {
        0
    };
#ifdef UNICODE
    WCHAR strExeName[MAX_PATH] =
#else
    CHAR strExeName[MAX_PATH] =
#endif
    {
        0
    };
#ifdef UNICODE
    WCHAR* strLastSlash = NULL;
#else
    CHAR* strLastSlash = NULL;
#endif
    GetModuleFileName( NULL, strExePath, MAX_PATH );
    strExePath[MAX_PATH - 1] = 0;
#ifdef UNICODE
    strLastSlash = wcsrchr( strExePath, TEXT( '\\' ) );
#else
    strLastSlash = strrchr( strExePath, TEXT( '\\' ) );
#endif
    if( strLastSlash )
    {
#ifdef UNICODE
        wcscpy_s( strExeName, MAX_PATH, &strLastSlash[1] );
#else

#endif
        // Chop the exe name from the exe path
        *strLastSlash = 0;

        // Chop the .exe from the exe name
#ifdef UNICODE
        strLastSlash = wcsrchr( strExeName, TEXT( '.' ) );
#else
        strLastSlash = strrchr( strExeName, TEXT( '.' ) );
#endif
        if( strLastSlash )
            *strLastSlash = 0;
    }

    // Search in directories:
    //      .\
    //      %EXE_DIR%\..\..\%EXE_NAME%
#ifdef UNICODE
    wcscpy_s( strDestPath, cchDest, strFilename );
#else
	strcpy_s( strDestPath, cchDest, strFilename );
#endif
    if( GetFileAttributes( strDestPath ) != 0xFFFFFFFF )
        return S_OK;

//    swprintf_s( strDestPath, cchDest, L"%s\\..\\..\\%s\\%s", strExePath, strExeName, strFilename );
#ifdef UNICODE
    swprintf_s( strDestPath, cchDest, L"%s\\..\\%s\\%s", strExePath, strExeName, strFilename );
#else
    sprintf_s( strDestPath, cchDest, "%s\\..\\%s\\%s", strExePath, strExeName, strFilename );
#endif
    if( GetFileAttributes( strDestPath ) != 0xFFFFFFFF )
        return S_OK;    

    // On failure, return the file as the path but also return an error code
#ifdef UNICODE
    wcscpy_s( strDestPath, cchDest, strFilename );
#else
    strcpy_s( strDestPath, cchDest, strFilename );
#endif

	ASSERT( 0 );

    return E_FAIL;
}
//---------------------------------------------------------------------------------------------------
template<>
KernelBuilder<TYPE_DX11>::~KernelBuilder()
{
	
}

template<>
void KernelBuilder<TYPE_DX11>::SetFromFile( const Device* pDeviceData, const char* pFileName, const char* pOption, bool bAddExtension,
	bool bCacheKernel)
{
	char fileNameWithExtension[256];

	if( bAddExtension )
	{
		sprintf_s( fileNameWithExtension, "%s.hlsl", pFileName );
	}
	else
	{
		sprintf_s( fileNameWithExtension, "%s", pFileName );
	}

	int nameLength = (int)strlen(fileNameWithExtension)+1;
#ifdef UNICODE
	WCHAR* wFileNameWithExtension = new WCHAR[ nameLength ];
#else
	CHAR* wFileNameWithExtension = new CHAR[ nameLength ];
#endif

	memset( wFileNameWithExtension, 0, nameLength );
#ifdef UNICODE
	MultiByteToWideChar( CP_ACP, 0, fileNameWithExtension, -1, wFileNameWithExtension, nameLength );
#else
	sprintf( wFileNameWithExtension, nameLength, "%s%", fileNameWithExtension );
#endif

	HRESULT hr;

	hr = FindDXSDKShaderFileCch( m_Path, MAX_PATH, wFileNameWithExtension );

	delete [] wFileNameWithExtension;

	ASSERT( hr == S_OK );
}

template<>
void KernelBuilder<TYPE_DX11>::SetFromSrc( const Device* pDeviceData, const char* pSrc, const char* pOption )
{
	m_pDevice = pDeviceData;
	m_pPtr = (void*)pSrc;
	m_Path[0] = '0';
}

template<>
void KernelBuilder<TYPE_DX11>::CreateKernel( const char* pFuncName, Kernel& kernelOut )
{
	const DeviceDX11* deviceData = (const DeviceDX11*)m_pDevice;
	KernelDX11* dxKernel = (KernelDX11*)&kernelOut;
	HRESULT hr;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] = 
	{
#ifdef USE_STRUCTURED_BUFFERS
		"USE_STRUCTURED_BUFFERS", "1",
#endif

#ifdef TEST_DOUBLE
		"TEST_DOUBLE", "1",
#endif
		NULL, NULL
	};

	// We generally prefer to use the higher CS shader profile when possible as CS 5.0 is better performance on 11-class hardware
	LPCSTR pProfile = ( deviceData->m_pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ) ? "cs_5_0" : "cs_4_0";

	ID3DBlob* pErrorBlob = NULL;
	ID3DBlob* pBlob = NULL;
	if( m_Path[0] == '0' )
	{
		char* src = (char*)m_pPtr;
		hr = D3DX11CompileFromMemory( src, strlen(src), 0, defines, NULL, pFuncName, pProfile, 
			dwShaderFlags, NULL, NULL, &pBlob, &pErrorBlob, NULL );
	}
	else
	{
		hr = D3DX11CompileFromFile( m_Path, defines, NULL, pFuncName, pProfile, 
			dwShaderFlags, NULL, NULL, &pBlob, &pErrorBlob, NULL );
	}

	if ( FAILED(hr) )
	{
		debugPrintf("%s", (char*)pErrorBlob->GetBufferPointer());
	}
	ASSERT( hr == S_OK );

	hr = deviceData->m_pDevice->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, 
		dxKernel->GetKernelPtr() );

#if defined(DEBUG) || defined(PROFILE)
	if ( kernelOut.m_pKernel )
	{
		//kernelOut.m_pKernel->SetPrivateData( WKPDID_D3DDebugObjectName, lstrlenA(pFuncName), pFuncName );
	}
#endif

	SAFE_RELEASE1( pErrorBlob );
	SAFE_RELEASE1( pBlob );

	kernelOut.m_eType = TYPE_DX11;
}

template<>
void KernelBuilder<TYPE_DX11>::DeleteKernel( Kernel& kernel )
{
	KernelDX11* dxKernel = (KernelDX11*)&kernel;

	if( kernel.m_pKernel )
	{
		dxKernel->GetKernel()->Release();
		kernel.m_pKernel = NULL;
	}
}

//---------------------------------------------------------------------------------------------------
struct LauncherDX11
{
	typedef Launcher::BufferInfo BufferInfo;

	__inline static void SetBuffers( Launcher* pLauncher, BufferInfo* pBuffInfo, int n );
	template<typename T> 
	__inline static void SetConst( Launcher* pLauncher, Buffer<T>& constBuff, const T& consts );
	__inline static void Launch2D( Launcher* pLauncher, int numThreadsX, int numThreadsY, int localSizeX, int localSizeY );
};

void LauncherDX11::SetBuffers( Launcher* pLauncher, BufferInfo* pBuffInfo, int n )
{

}

template<typename T>
void LauncherDX11::SetConst( Launcher* pLauncher, Buffer<T>& constBuff, const T& consts )
{

}

void LauncherDX11::Launch2D( Launcher* pLauncher, int numThreadsX, int numThreadsY, int localSizeX, int localSizeY )
{
	KernelDX11* dxKernel = (KernelDX11*)pLauncher->m_pKernel;
	const DeviceDX11* pDevice = (const DeviceDX11*)pLauncher->m_pDeviceData;

	pDevice->m_pContext->CSSetShader( dxKernel->GetKernel(), NULL, 0 );

	int nx, ny, nz;

	nx = max( 1, ( numThreadsX/localSizeX)+(!(numThreadsX%localSizeX)?0:1) );
	ny = max( 1, ( numThreadsY/localSizeY)+(!(numThreadsY%localSizeY)?0:1) );
	nz = 1;

	pDevice->m_pContext->CSSetShader( NULL, NULL, 0 );

	if( pLauncher->m_IdxRw )
	{
		ID3D11UnorderedAccessView* aUAViewsNULL[ 16 ] = { 0 };
		pDevice->m_pContext->CSSetUnorderedAccessViews( 0, 
			min( (unsigned int)pLauncher->m_IdxRw, sizeof(aUAViewsNULL)/sizeof(*aUAViewsNULL) ), 
			aUAViewsNULL, NULL );
	}
	if( pLauncher->m_Idx )
	{
		ID3D11ShaderResourceView* ppSRVNULL[ 16 ] = { 0 };
		pDevice->m_pContext->CSSetShaderResources( 0, 
			min( (unsigned int)pLauncher->m_Idx, sizeof(ppSRVNULL)/sizeof(*ppSRVNULL) ), ppSRVNULL );
	}
}

#undef SAFE_RELEASE1

};