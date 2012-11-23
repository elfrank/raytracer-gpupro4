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

#ifndef __DEVICE_HPP__
#define __DEVICE_HPP__

namespace raytracer
{

enum DeviceType
{
	TYPE_CL = 0,
	TYPE_DX11 = 1,
	TYPE_HOST,
};

class Device;

struct DeviceUtils
{
	struct Config
	{
		enum DeviceType
		{
			DEVICE_GPU,
			DEVICE_CPU,
		};

		enum DeviceVendor
		{
			VD_AMD,
			VD_INTEL,
			VD_NVIDIA,
		};

		Config() : m_eType( DEVICE_GPU ), m_iDeviceIdx( 0 ), m_eVendor( VD_NVIDIA ) {}

		DeviceType m_eType;
		DeviceVendor m_eVendor;
		int m_iDeviceIdx;
	};	

	__inline static int GetNumberOfDevices( DeviceType eType );
	__inline static Device* Allocate( DeviceType eType, Config& cfg );
	__inline static void Deallocate( Device* pDeviceData );
	__inline static void WaitForCompletion( const Device* pDeviceData );
};

struct Kernel;

class Device
{
public:
	typedef DeviceUtils::Config Config;
	DeviceType	m_eType;
	size_t	m_tMemoryUsage;

	Device( DeviceType eType ) : m_eType( eType ), m_tMemoryUsage( 0 ) {}
	//virtual ~Device(void);

	virtual void*	GetDevice() const { return 0; }
	virtual void*	GetContext() const { return 0; }
	virtual void	Initialize( const Config& config ) {}
	virtual void	Release( void ) {}
	virtual void	WaitForCompletion() const {}
	virtual void	GetDeviceName( char cNameOut[128] ) const {}
	virtual Kernel*	GetKernel( const char* pFileName, const char* pFunctionName, 
								const char* pOption = NULL, const char* pSrc = NULL, 
								bool bCacheKernel = true ) const { return 0; }
};

};

#endif