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

#ifndef __KERNEL_HPP__
#define __KERNEL_HPP__

#include <map>
#include <string>
#include <fstream>

namespace raytracer
{

struct Kernel
{
	DeviceType m_eType;
	void* m_pKernel;
};

struct KernelManager
{
	typedef std::map<std::string,Kernel*> KMap;

	__inline ~KernelManager();
	__inline Kernel* Query(const Device* pDevice, const char* pFileName, const char* pFuncName, const char* pOption = NULL, const char* pSrc = NULL, bool bCacheKernel = true );

	KMap m_Map;
};

struct Launcher
{
	struct BufferInfo
	{
		template<typename T> BufferInfo( Buffer<T> buff, bool bIsReadOnly = false ) :
			m_pBuffer( buff ), m_bIsReadOnly( bIsReadOnly) {}
		
		void* m_pBuffer;
		bool m_bIsReadOnly;
	};
	
	__inline Launcher( const Device* pDevice, char* pFileName, char* pFuncName, char* pOption = NULL );
	__inline Launcher( const Device* pDevice, Kernel* pKernel );
	__inline void SetBuffers( BufferInfo* pBufferInfo, int n );
	template<typename T> __inline void SetConst( Buffer<T>& constBuff, const T& consts );
	__inline void Launch1D( int numThreads, int localSize = 64 );
	__inline void Launch2D( int numThreadsX, int numThreadsY, int localSizeX = 8, int localSizeY = 8 );
	

	enum
	{
		CONST_BUFFER_SIZE = 512,
	};

	const Device* m_pDeviceData;
	Kernel* m_pKernel;
	int m_Idx;
	int m_IdxRw;
};

template<DeviceType TYPE>
class KernelBuilder
{
public:
	__inline KernelBuilder(): m_pPtr( 0 ) {}
	__inline ~KernelBuilder();
	__inline static void DeleteKernel( Kernel& kernel );
	
	__inline void SetFromFile( const Device* pDevice, const char* pFileName,
								const char* pOption = NULL, bool bAddExtension = false,
								bool bCacheKernel = true );
	__inline void SetFromSrc( const Device* pDevice, const char* pSrc, const char* pOption = NULL );
	__inline void CreateKernel( const char* pFuncName, Kernel& kernelOut );

private:
	enum
	{
		MAX_PATH_LENGTH = 260,
	};
	const Device* m_pDevice;
#ifdef UNICODE
	wchar_t m_Path[ MAX_PATH_LENGTH ];
#else
	char_t m_Path[ MAX_PATH_LENGTH ];
#endif
	void* m_pPtr;
};

};

#endif