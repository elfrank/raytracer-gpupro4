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

#if defined(ENABLE_DX11)
	#if defined(ENABLE_CL)
	#define SELECT_DEVICEDATA( type, func ) \
		switch( type ) \
		{ \
		case TYPE_CL: ((DeviceCL*)m_pDevice)->func; break; \
		case TYPE_DX11: ((DeviceDX11*)m_pDevice)->func; break; \
		case TYPE_HOST: ((DeviceHost*)m_pDevice)->func; break; \
		default: ADLASSERT(0); break; \
		}

	#define SELECT_DEVICEDATA1( deviceData, func ) \
		switch( deviceData->m_type ) \
		{ \
		case TYPE_CL: ((DeviceCL*)deviceData)->func; break; \
		case TYPE_DX11: ((DeviceDX11*)deviceData)->func; break; \
		case TYPE_HOST: ((DeviceHost*)deviceData)->func; break; \
		default: ADLASSERT(0); break; \
		}
	#else
	#define SELECT_DEVICEDATA( type, func ) \
		switch( type ) \
		{ \
		case TYPE_DX11: ((DeviceDX11*)m_pDevice)->func; break; \
		default: ASSERT(0); break; \
		}

	#define SELECT_DEVICEDATA1( deviceData, func ) \
		switch( deviceData->m_type ) \
		{ \
		case TYPE_DX11: ((DeviceDX11*)m_pDevice)->func; break; \
		default: ASSERT(0); break; \
		}
	#endif
#else
	#if defined(ENABLE_CL)
	#define SELECT_DEVICEDATA( type, func ) \
		switch( type ) \
		{ \
		case TYPE_CL: ((DeviceCL*)m_device)->func; break; \
		case TYPE_HOST: ((DeviceHost*)m_device)->func; break; \
		default: ADLASSERT(0); break; \
		}

	#define SELECT_DEVICEDATA1( deviceData, func ) \
		switch( deviceData->m_type ) \
		{ \
		case TYPE_CL: ((DeviceCL*)deviceData)->func; break; \
		case TYPE_HOST: ((DeviceHost*)deviceData)->func; break; \
		default: ADLASSERT(0); break; \
		}
	#else
	#define SELECT_DEVICEDATA( type, func ) \
		switch( type ) \
		{ \
		case TYPE_HOST: ((DeviceHost*)m_device)->func; break; \
		default: ADLASSERT(0); break; \
		}

	#define SELECT_DEVICEDATA1( deviceData, func ) \
		switch( deviceData->m_type ) \
		{ \
		case TYPE_HOST: ((DeviceHost*)deviceData)->func; break; \
		default: ADLASSERT(0); break; \
		}
	#endif
#endif


};