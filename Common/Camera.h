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

// ------------------------------------------------
// Camera.h
// ------------------------------------------------
// Manipulates the camera on the scene.

#ifndef CAMERA_H
#define CAMERA_H

#include "Common.h"
#include "Geometry.h"
#include "Matrix4.h"

class Camera
{
private:
	Matrix4	m_mInverseMatrix;
	float	m_fSpeed;		// Movemente speed
	
	// Controls the movement of the camera.
	bool	m_bFront;		// W									
	bool	m_bBack;		// S						
	bool	m_bLeft;		// A							
	bool	m_bRight;		// D							
	bool	m_bUp;			// Q						
	bool	m_bDown;		// E							
	bool	m_bPause;		// P
	bool	m_bOrbit;
public:
	Camera() {}
	Camera(Point& pos, float speed);
	~Camera(void);

	// Functions
	bool	Move(float fTime);											// Moves the Camera using the keyboard
	void	Update();													// Updates the position of the camera every frame
	void	Rotate(int x, int y);										// Rotates the camera using the mouse
	void	IncreaseSpeed(void);
	void	DecreaseSpeed(void);
	void	SetPosition(Point &a_Pos);
	void	Turn( int axis, float angle );
	void	SetCamera( unsigned int a_iOption ) ;
	void	Orbit( float a_fTimer);
	void	LookAt(const Vector3& dir, const Vector3& up, Matrix4& m); 

	void	ChangeOrbitingState() { m_bOrbit = !m_bOrbit; printf("Orbiting is %s\n", m_bOrbit?"ON":"OFF"); }
	void	ChangePausingState() { m_bPause = !m_bPause; printf("Pause is %s\n", m_bPause?"ON":"OFF"); }

	// Getters
	Vector3 GetPosition() { return Vector3(m_mInverseMatrix.xt, m_mInverseMatrix.yt, m_mInverseMatrix.zt);}
	Matrix4	GetInverseMatrix() { return m_mInverseMatrix; }
	float	GetSpeed() { return m_fSpeed; }
	bool	IsFront() { return m_bFront; }
	bool	IsBack() { return m_bBack; }
	bool	IsLeft() { return m_bLeft; }
	bool	IsRight() { return m_bRight; }
	bool	IsUp() { return m_bUp; }
	bool	IsDown() { return m_bDown; }
	bool	IsPaused() { return m_bPause; }
	bool	IsOrbiting() { return m_bOrbit; }

	// Setters
	void	SetFront( bool bValue ) { m_bFront = bValue; }
	void	SetBack( bool bValue ) { m_bBack = bValue; }
	void	SetLeft( bool bValue ) { m_bLeft = bValue; }
	void	SetRight( bool bValue ) { m_bRight = bValue; }
	void	SetUp( bool bValue ) { m_bUp = bValue; }
	void	SetDown( bool bValue ) { m_bDown = bValue; }
	void	SetPause( bool bValue ) { m_bPause = bValue; }
	void	SetOrbit( bool bValue ) { m_bOrbit = bValue; }
};

#endif