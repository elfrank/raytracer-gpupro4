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

// Helper class for time tracking of the app.

#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#ifdef WINDOWS
#include "windows.h"
#elif defined(LINUX)
#include <DataTypes.h>
#endif
#include <stdio.h>

class Performance
{
private:
	UINT						m_iFrameCount;											// Frame count for FPS
	LARGE_INTEGER				m_Timer;												// Timer for animation
	LARGE_INTEGER				m_FPSTimer;												// Timer for FPS
	LARGE_INTEGER				m_Timer_Frequency;										// Timer frequency (needed for tick to second convertion)

	LARGE_INTEGER				m_TimerRender;											// Timer for render
	LARGE_INTEGER				m_TimerFrequencyRender;									// Timer frequency (needed for tick to second convertion)

	__int64						m_iNumCandidates;
	__int64						m_iNumRays;												// Number of rays
public:
	// Constructor
	Performance(void);
	// Destructor
	~Performance(void);

	// Functions
	bool						updateFPS(char *aux, char *name);						// Update frames per second
	float						updateTime(void);										// Update current time
	float						updateTimeRender(void);									// Update render time (construct frame)
	void						addCandidates(__int64 n) { m_iNumCandidates += n; }
	void						addRays(__int64 n) { m_iNumRays += n; }

	// Getters
	UINT						getFrameCount() { return m_iFrameCount; }
	LARGE_INTEGER				getTimer() { return m_Timer; }
	LARGE_INTEGER				getFPSTimer() { return m_FPSTimer; }
	LARGE_INTEGER				getTimerFrequency() { return m_Timer_Frequency; }
	__int64						getNumCandidates() { return m_iNumCandidates; }
	__int64						getNumRays() { return m_iNumRays; }

	// Setters
	void						setNumCandidates(__int64 n) { m_iNumCandidates = n; }
	void						setNumRays(__int64 n) { m_iNumRays = n; }

};

#endif