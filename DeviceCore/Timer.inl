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

void Timer::Init( const Device* pDevice )
{
	if( pDevice )
	{
		switch( pDevice->m_eType )
		{
		case TYPE_DX11:
			m_pImpl = new TimerDX11;
			break;
		}
	}
	m_pImpl->Init( pDevice );
}

Timer::~Timer()
{
	if( m_pImpl == 0 )
		return;
	delete m_pImpl;
}

};