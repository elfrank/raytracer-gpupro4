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

#ifndef __BUFFER_H__
#define __BUFFER_H__

namespace raytracer
{

struct BufferBase
{
	enum BufferType
	{
		BUFFER,

		/* dx */
		BUFFER_CONST,
		BUFFER_STAGING,
		BUFFER_APPEND,
		BUFFER_RAW,
		BUFFER_W_COUNTER,
		BUFFER_INDEX,
		BUFFER_VERTEX,

		/* cl */
		BUFFER_ZERO_COPY,
	};
};

//template<typename T>

template<typename T>
struct Buffer: public BufferBase
{
private:
	

public:
	const Device*	m_pDevice;
	size_t			m_tSize;
	T*				m_pPtr;
	/* dx */
	void*			m_pUAV;
	void*			m_pSRV;
	bool			m_bAllocated;

	__inline Buffer();
	__inline Buffer( const Device* pDevice, int nElems, BufferType eType = BUFFER );
	__inline virtual ~Buffer();

	__inline void SetRawPtr( const Device* pDevice, T* pPtr, int iSize, BufferType eType = BUFFER );
	__inline void Allocate( const Device* pDevice, int nElems, BufferType eType = BUFFER );
	__inline void Deallocate( const Device* pDevice );
	__inline void Write( T* pHostSrcPtr, int nElems, int iDestOffsetNElems = 0 );
	__inline void Read( T* pHostDestPtr, int nElems, int srcOffsetNElems = 0 ) const;
	__inline void Write( Buffer<T>& src, int nElems );
	__inline void Read( Buffer<T>& dest, int nElems ) const;
	__inline int GetSize() const { return m_tSize; } ;
	DeviceType GetType() const { return m_pDevice->GetType(); }
};

//struct DeviceType;
class Device;
struct BufferUtils
{
	template<DeviceType TYPE, bool COPY, typename T>
	__inline static typename Buffer<T>* Map( const Device* pDevice,
											const Buffer<T>* pIn,
											int copySize = -1 );

	template<bool COPY, typename T>
	__inline static void Unmap( Buffer<T>* pNative, const Buffer<T>* pOrig,
								int copySize = 1 );
};

};

#endif