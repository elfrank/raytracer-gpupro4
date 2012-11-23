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

#ifndef __SCENE_H__
#define __SCENE_H__

#include "Object.h"
#include "Model.h"

class Scene
{
private:
	Object**					m_pObjects;
	Model**						m_pModels;
	AccelerationStructure*		m_pAccelStructure;

	unsigned int				m_uiNumObjects;
	unsigned int				m_uiNumModels;
public:
	Scene(std::map<string,vector<Point>> &a_FileNames);
	~Scene();

	void						Rebuild();
	void						ChangeStructure(ACCELERATION_STRUCTURE a_Structure);

	// Getters
	Object**					GetObjects( void )			{ return m_pObjects; }
	Model**						GetModels( void )			{ return m_pModels; }
	AccelerationStructure*		GetAccelStructure( void )	{ return m_pAccelStructure; }
	unsigned int				GetNumObjects( void )		{ return m_uiNumObjects; }
	unsigned int				GetNumModels( void )		{ return m_uiNumModels; }
};

#endif