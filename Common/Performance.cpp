// ================================================================================ //
// The authors of this code (Francisco Ávila, Sergio Murguía, Leo Reyes and Arturo	//
// García) hereby release it into the public domain. This applies worldwide.		//
//																					//
// In case this is not legally possible, we grant any entity the right to use this	//
// work for any purpose, without any conditions, unless such conditions are			//
// required by law.																	//
//																					//
// This work is provided on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF	//
// ANY KIND. You are solely responsible for the consequences of using or			//
// redistributing this code and assume any risks associated with these or related	//
// activities.																		//
// ================================================================================ //

#include "Performance.h"

#ifdef LINUX
#include <sys/time.h>
#endif

//------------------------------------------------
// Constructor
//------------------------------------------------
Performance::Performance(void)
{
	m_iFrameCount = 0;
	m_iNumCandidates = 0;
	m_iNumRays = 0;

	//Initialize timer counters
#ifdef WINDOWS
	QueryPerformanceCounter(&m_Timer);
	m_FPSTimer = m_Timer;
	QueryPerformanceFrequency(&m_Timer_Frequency);

	QueryPerformanceCounter(&m_TimerRender);
	QueryPerformanceFrequency(&m_TimerFrequencyRender);
#elif defined(LINUX)
	timeval t1;
	gettimeofday(&t1,NULL);
    m_FPSTimer = m_Timer =t1.tv_sec + (t1.tv_usec/1000000.0);

	timeval t2;
	gettimeofday(&t2,NULL);
    m_TimerRender =t2.tv_sec + (t2.tv_usec/1000000.0);
#endif
}

//------------------------------------------------
// Updates Frames per second (FPS)
//------------------------------------------------
bool Performance::updateFPS(char *aux, char *name)
{
	LARGE_INTEGER Timer;
#ifdef WINDOWS
	QueryPerformanceCounter(&Timer);
	m_iFrameCount++;
	float step = float(Timer.QuadPart-m_FPSTimer.QuadPart)/float(m_Timer_Frequency.QuadPart);
#elif defined(LINUX)
		timeval t1;
	gettimeofday(&t1,NULL);
    Timer = t1.tv_sec + (t1.tv_usec/1000000.0);
	m_iFrameCount++;
	float step = Timer-m_FPSTimer;
#endif

	if ( (m_iFrameCount>=100) || (step>=1.0) )
	{
		//sprintf_s(aux,1024,"FPS: %f (%f spf) with up to %d reflections", float(m_iFrameCount)/step,step/float(m_iFrameCount),m_Effect->GetNumReflexes()-1);
		//sprintf_s(aux,1024,"%s FPS: %lf (%lf spf), %d/%d threads", name, float(m_FrameCount)/step,step/float(m_FrameCount),m_iNumThreads,m_numCPU);
#ifdef WINDOWS
		sprintf_s(aux,1024,"%s FPS: %lf (%lf spf)", name, float(m_iFrameCount)/step,step/float(m_iFrameCount));
#elif defined(LINUX)
		sprintf(aux,"%s FPS: %lf (%lf spf)", name, float(m_iFrameCount)/step,step/float(m_iFrameCount));
#endif
		m_iFrameCount = 0;
		m_FPSTimer = Timer;
		return true;
	}
	return false;
}

//------------------------------------------------
// Function to meassure global time (used for
// animation).
//------------------------------------------------
float Performance::updateTime( void )
{
	LARGE_INTEGER Timer;
#ifdef WINDOWS
	QueryPerformanceCounter(&Timer);
	float step = float(Timer.QuadPart-m_Timer.QuadPart)/float(m_Timer_Frequency.QuadPart);
#elif defined(LINUX)
	timeval t1;
	gettimeofday(&t1,NULL);
    Timer = t1.tv_sec + (t1.tv_usec/1000000.0);
	float step = Timer - m_Timer;
#endif
	m_Timer=Timer;
	return step;
}

//------------------------------------------------
// Function to meassure the time neccesary to
// render a single frame
//------------------------------------------------
float Performance::updateTimeRender( void )
{
	LARGE_INTEGER Timer;
#ifdef WINDOWS
	QueryPerformanceCounter(&Timer);
	float step = float(Timer.QuadPart-m_TimerRender.QuadPart)/float(m_TimerFrequencyRender.QuadPart);
#elif defined(LINUX)
	timeval t1;
	gettimeofday(&t1,NULL);
    Timer = t1.tv_sec + (t1.tv_usec/1000000.0);
	float step = Timer - m_TimerRender;
#endif
	m_TimerRender=Timer;
	return step;
}

//------------------------------------------------
// Destructor
//------------------------------------------------
Performance::~Performance(void)
{
}
