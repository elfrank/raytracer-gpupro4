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

#ifdef ENABLE_DX11
	//#include <DeviceCore/DX11/KernelUtilsDX11.inl>
#endif


#include <DeviceCore/DX11/KernelDX11.inl>

namespace raytracer
{

Kernel* KernelManager::Query( const Device* pDevice, const char* pFileName, const char* pFuncName, const char* pOption, const char* pSrc, bool bCacheKernel )
{
	const int charSize = 1024*2;
	KernelManager* s_kManager = this;

	char fullFineName[charSize];
	switch( pDevice->m_eType )
	{
		case TYPE_CL:
#if defined(ENABLE_CL)
		sprintf_s(fullFineName, charSize, %s.cl", pFileName);
		break;
#endif
#if defined(ENABLE_DX11)
		sprintf_s( fullFineName, charSize, "%s.hlsl", pFileName );
		break;
#endif
		default:
			ASSERT(0);
			break;
	};

	char mapName[ charSize ];
	if( pOption )
	{
		sprintf_s( mapName, charSize, "%d%s%s%s", (int)pDevice->GetContext(), fullFineName, pFuncName, pOption );
	}
	else
	{
		sprintf_s( mapName, charSize, "d%s%s", (int)pDevice->GetContext(), fullFineName, pFuncName );
	}

	std::string str( mapName );

	KMap::iterator iter = s_kManager->m_Map.find( str );

	Kernel* kernelOut;
	if( iter == s_kManager->m_Map.end() )
	{
		kernelOut = new Kernel();

		switch( pDevice->m_eType )
		{
#if defined (ENABLE_CL)
			case TYPE_CL:
				KernelBuilder<TYPE_CL> builder;
				if( pSrc )
				{
					builder.SetFromSrc( pDevice, pSrc, pOption );
				}
				else
				{
					builder.SetFromFile( pDevice, pFileName, pOption, true, bCacheKernel );
				}
				builder.CreateKernel( pFunctionName, *kernelOut );
				break;
#endif

			case TYPE_DX11:
#if defined( ENABLE_DX11 )
				KernelBuilder<TYPE_DX11> builder;
				if( pSrc )
				{
					builder.SetFromSrc( pDevice, pSrc, pOption );
				}
				else
				{
					builder.SetFromFile( pDevice, pFileName, pOption, true, bCacheKernel );
				}
				builder.CreateKernel( pFuncName, *kernelOut );
				break;
#endif
			/*default:
				ASSERT( 0 );
				break;*/
		};
		s_kManager->m_Map.insert( KMap::value_type( str, kernelOut ) );
	}
	else
	{
		kernelOut = iter->second;
	}

	return kernelOut;
}

KernelManager::~KernelManager()
{
	for( KMap::iterator iter = m_Map.begin(); iter != m_Map.end(); iter++ )
	{
		Kernel* k = iter->second;
		switch( k->m_eType )
		{
#if defined( ENABLE_CL )
			case TYPE_CL:
				KernelBuilder<TYPE_CL>::DeleteKernel( *k );
				break;
#endif
#if defined( ENABLE_DX11 )
			case TYPE_DX11:
				KernelBuilder<TYPE_DX11>::DeleteKernel( *k );
				break;
#endif
			case TYPE_HOST:
				break;
			default:
				ASSERT( 0 );
				break;
		};
	}
}

//==========================
//	Launcher
//==========================

#if defined(ENABLE_DX11)
	#if defined(ENABLE_CL)
	#define SELECT_LAUNCHER( type, func ) \
		switch( type ) \
		{ \
		case TYPE_CL: LauncherCL::func; break; \
		case TYPE_DX11: LauncherDX11::func; break; \
		default: ASSERT(0); break; \
		};
	#else
	#define SELECT_LAUNCHER( type, func ) \
		switch( type ) \
		{ \
		case TYPE_DX11: LauncherDX11::func; break; \
		default: ASSERT(0); break; \
		};
	#endif
#else
	#if defined(ENABLE_CL)
	#define SELECT_LAUNCHER( type, func ) \
		switch( type ) \
		{ \
		case TYPE_CL: LauncherCL::func; break; \
		default: ASSERT(0); break; \
		};
	#else
	#define SELECT_LAUNCHER( type, func ) \
		switch( type ) \
		{ \
		default: ASSERT(0); break; \
		};
	#endif
#endif

Launcher::Launcher( const Device* pDevice, char *pFileName, char *pFunctionName, char* pOption )
{
	m_pKernel = pDevice->GetKernel( pFileName, pFunctionName, pOption );
	m_pDeviceData = pDevice;
	m_Idx = 0;
	m_IdxRw = 0;
}

Launcher::Launcher( const Device* pDevice, Kernel* pKernel )
{
	m_pKernel = pKernel;
	m_pDeviceData = pDevice;
	m_Idx = 0;
	m_IdxRw = 0;
}

void Launcher::SetBuffers( BufferInfo* pBufferInfo, int n )
{
	SELECT_LAUNCHER( m_pDeviceData->m_eType, SetBuffers( this, pBufferInfo, n ) );
}

template<typename T>
void Launcher::SetConst( Buffer<T>& constBuffer, const T& consts )
{
	SELECT_LAUNCHER( m_pDeviceData->m_eType, SetConst( this, constBuffer, consts ) );
}

void Launcher::Launch1D( int numThreads, int localSize )
{
	SELECT_LAUNCHER( m_pDeviceData->m_eType, Launch2D( this, numThreads, 1, localSize, 1 ) );
}

void Launcher::Launch2D( int numThreadsX, int numThreadsY, int localSizeX, int localSizeY )
{
	SELECT_LAUNCHER( m_pDeviceData->m_eType, Launch2D( this, numThreadsX, numThreadsY, localSizeX, localSizeY ) );
}

};