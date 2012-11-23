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

#ifdef WINDOWS

#include "ArgumentsParser.h"
#include "Input.h"

// -----------------------------------------------------------
// Constructor
// -----------------------------------------------------------
Input::Input()
{
	m_bMouseDown = false;
	m_bShadows = true;								// Cast shadows?
	m_bPhongShading = true;							// Use Phong or flat shading
	m_bMoveLights = false;							// indicates to move the lights instead of cameras
	m_bMuliplicativeReflex = false;
	m_bReflections = true;
	m_iEnvMappingFlag = -1;
	m_bNormalMapping = true;
	m_bGlossMapping = true;
	//m_bRebuildStructure = m_Parser.IsDynamic();
	m_bRebuildStructure = false;
	m_iAccelerationStructureFlag = 0;
	m_iNumBounces = 0;
	m_bMouseDown = 0;
	m_MouseX = 0;
	m_MouseY = 0;
}

// -----------------------------------------------------------
// Destructor
// -----------------------------------------------------------
Input::~Input(void)
{
}

// -----------------------------------------------------------
// Mouse-Move, if left click is pressed, then rotate
// camera
// -----------------------------------------------------------
BOOL Input::OnMouseMove(UINT& x, UINT& y, LPARAM& lParam)
{
	x = m_MouseX;
	y = m_MouseY;
	m_MouseX = LOWORD(lParam);
	m_MouseY = HIWORD(lParam);
	x -= m_MouseX;
	y -= m_MouseY;
	return true;
}



#endif //WINDOWS