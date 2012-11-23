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

#ifndef __TIMER_H__
#define __TIMER_H__

namespace raytracer
{

struct TimerBase
{
	enum
	{
		CAPACITY = 64,
	};
	
	const Device* m_pDevice;
	int m_idx;

	__inline TimerBase(): m_pDevice(0) {}
	__inline TimerBase( const Device* pDevice ) { Init( pDevice );}
	__inline virtual ~TimerBase() {}

	__inline virtual void Init( const Device* pDevice ) = 0;
	__inline virtual void Start() = 0;
	__inline virtual void Split() = 0;
	__inline virtual void Stop() = 0;
	__inline virtual float GetMs() = 0;
	__inline virtual void GetMs( float* times, int capacity ) = 0;
	__inline virtual int GetNIntervals() const { return m_idx-1; }

	
};

class Timer : public TimerBase
{
	__inline Timer( const Device* pDevice = NULL ) { m_pImpl = 0; if( pDevice ) Init(pDevice); }
	__inline ~Timer();

	__inline void Init( const Device* pDevice );
	__inline void Start() { if(!m_pImpl ) Init(0); m_pImpl->Start(); }
	__inline void Split() { m_pImpl->Split(); }
	__inline void Stop() { m_pImpl->Stop(); }
	__inline float GetMs() { return m_pImpl->GetMs(); }
	__inline void GetMs( float* times, int capacity ) { m_pImpl->GetMs(times,capacity); }
	__inline int GetNIntervals() const { return m_pImpl->GetNIntervals(); }

	TimerBase* m_pImpl;
};

};

#endif