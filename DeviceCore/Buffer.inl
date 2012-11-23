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

template<typename T>
Buffer<T>::Buffer()
{
	m_pDevice = 0;
	m_tSize = 0;
	m_pPtr = 0;

	m_pUAV = 0;
	m_pSRV = 0;

	m_bAllocated = false;
}

template<typename T>
Buffer<T>::Buffer( const Device* pDeviceData, int nElems, BufferType eType )
{
	m_pDevice = 0;
	Allocate( pDeviceData, nElems, eType );
}

template<typename T>
Buffer<T>::~Buffer()
{
	if( m_bAllocated )	
	{
		if( m_pDevice )
		{
			SELECT_DEVICEDATA( m_pDevice->m_eType, Deallocate( this ) );
		}
	}
}

template<typename T>
void Buffer<T>::Allocate( const Device* pDeviceData, int nElems, BufferType eType )
{
	ASSERT( m_pDevice == 0 );
	m_pDevice = pDeviceData;
	m_tSize = 0;
	m_pPtr = 0;
	
	m_pUAV = 0;
	m_pSRV = 0;

	SELECT_DEVICEDATA( m_pDevice->m_eType, Allocate( this, nElems, eType ) );
}

template<typename T>
void Buffer<T>::Deallocate( const Device* pDeviceData )
{
	ASSERT( m_pDevice->GetUsedMemory() == 0 );
	m_pDevice->Release();
	delete m_pDevice;
}

template<typename T>
void Buffer<T>::SetRawPtr( const Device* pDevice, T* pPtr, int tSize, BufferType eType )
{
	ASSERT( pDevice == 0 );
	ASSERT( eType == BUFFER );
	ASSERT( pDevice->m_eType != TYPE_DX11 );

	m_pDevice = pDevice;
	m_pPtr = pPtr;
	m_tSize = tSize;
}

template<typename T>
void Buffer<T>::Write( T* hostPtr, int nElems, int offsetNElems )
{
	ASSERT( nElems + offsetNElems <= m_tSize );
	SELECT_DEVICEDATA( m_pDevice->m_eType, Copy( this, hostPtr, nElems, offsetNElems ) );
}

template<typename T>
void Buffer<T>::Read( T* hostPtr, int nElems, int offsetNElems ) const
{
	SELECT_DEVICEDATA( m_pDevice->m_eType, Copy(hostPtr, this, nElems, offsetNElems ) );
}

template<typename T>
void Buffer<T>::Read( Buffer<T>& dest, int nElems ) const
{
	SELECT_DEVICEDATA( m_pDevice->m_eType, Copy(&dest, this, nElems ) );
}

template<typename T>
void Buffer<T>::Write( Buffer<T>& src, int nElems )
{
	ASSERT( nElems <= m_tSize );
	SELECT_DEVICEDATA( m_pDevice->m_eType, Copy( this, &src, nElems ) );
}

};