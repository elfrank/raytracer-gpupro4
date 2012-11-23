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

struct TimerDX11 : public TimerBase
{
	__inline TimerDX11() : TimerBase() {}
	__inline ~TimerDX11();

	__inline void Init( const Device* pDevice );
	__inline void Start();
	__inline void Split();
	__inline void Stop();
	__inline float GetMs();
	__inline void GetMs( float* times, int capacity );

	ID3D11Query* m_tQuery[CAPACITY+1];
	ID3D11Query* m_fQuery;
	UINT64 m_t[CAPACITY];
};

TimerDX11::~TimerDX11(void)
{
	m_fQuery->Release();
	for(int i=0; i<CAPACITY+1; i++)
	{
		m_tQuery[i]->Release();
	}
}

void TimerDX11::Init( const Device* pDevice )
{
	m_pDevice = pDevice;

	D3D11_QUERY_DESC qDesc;
	qDesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	qDesc.MiscFlags = 0;
	((ID3D11Device*)((const DeviceDX11*)m_pDevice)->GetDevice())->CreateQuery( &qDesc, &m_fQuery );

	for(int i=0; i<CAPACITY+1; i++)
	{
		D3D11_QUERY_DESC qDesc;
		qDesc.Query = D3D11_QUERY_TIMESTAMP;
		qDesc.MiscFlags = 0;
		((ID3D11Device*)((const DeviceDX11*)m_pDevice)->GetDevice())->CreateQuery( &qDesc, &m_tQuery[i] );
	}
}

void TimerDX11::Start( void )
{
	m_idx = 0;
	((ID3D11DeviceContext*)((const DeviceDX11*)m_pDevice)->GetContext())->Begin( m_fQuery );
	((ID3D11DeviceContext*)((const DeviceDX11*)m_pDevice)->GetContext())->End( m_tQuery[m_idx++] );
}

void TimerDX11::Stop( void )
{
	((ID3D11DeviceContext*)((const DeviceDX11*)m_pDevice)->GetContext())->End( m_tQuery[m_idx++] );
	((ID3D11DeviceContext*)((const DeviceDX11*)m_pDevice)->GetContext())->End( m_fQuery );
}

float TimerDX11::GetMs()
{
	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT d;
//	m_deviceData->m_context->End( m_fQuery );
	while( ((ID3D11DeviceContext*)((const DeviceDX11*)m_pDevice)->GetContext())->GetData( m_fQuery, &d,sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT),0 ) == S_FALSE ) {}
	while( ((ID3D11DeviceContext*)((const DeviceDX11*)m_pDevice)->GetContext())->GetData( m_tQuery[0], &m_t[0],sizeof(UINT64),0 ) == S_FALSE ){}
	while( ((ID3D11DeviceContext*)((const DeviceDX11*)m_pDevice)->GetContext())->GetData( m_tQuery[1], &m_t[1],sizeof(UINT64),0 ) == S_FALSE ){}

	//ADLASSERT( d.Disjoint == false );

	float elapsedMs = (m_t[1] - m_t[0])/(float)d.Frequency*1000;
	return elapsedMs;
}

void TimerDX11::GetMs( float* times, int capacity )
{
	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT d;
	while( ((ID3D11DeviceContext*)((const DeviceDX11*)m_pDevice)->GetContext())->GetData( m_fQuery, &d, sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT),0 ) == S_FALSE ) {}

	for( int i = 0; i < m_idx; ++i )
	{
		while( ((ID3D11DeviceContext*)((const DeviceDX11*)m_pDevice)->GetContext())->GetData( m_tQuery[i], &m_t[i], sizeof(UINT64), 0 ) ==S_FALSE ) {}
	}

	for( int i = 0; i < capacity; ++i )
	{
		times[i] = (m_t[i+1] - m_t[i])/(float)d.Frequency*1000;
	}
}

};