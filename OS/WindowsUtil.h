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

#ifndef __WINDOWSUTIL_H__
#define __WINDOWSUTIL_H__
#ifdef WINDOWS

#include "windows.h"
#include <string>

/* This function is needed to convert from *wchar_t to a std::string */
inline bool cvtLPW2stdstring(std::string& s, const LPWSTR pw, UINT codepage = CP_ACP)
{
    bool res = false;
    char* p = 0;
    int bsz;
 
    bsz = WideCharToMultiByte(codepage,
        0,
        pw,-1,
        0,0,
        0,0);
    if (bsz > 0) {
        p = new char[bsz];
        int rc = WideCharToMultiByte(codepage,
            0,
            pw,-1,
            p,bsz,
            0,0);
        if (rc != 0) {
            p[bsz-1] = 0;
            s = p;
            res = true;
        }
    }
    delete [] p;
    return res;
}

inline float UpdateTime( void )
{
	static DWORD dwTimeStart = 0;
    DWORD dwTimeCur = GetTickCount();
    if( dwTimeStart == 0 )
		dwTimeStart = dwTimeCur;
	float time = ( dwTimeCur - dwTimeStart ) / 1000.0f;
	return time;
}

#endif
#endif