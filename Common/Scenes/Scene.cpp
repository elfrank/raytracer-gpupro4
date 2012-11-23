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

#include "Scene.h"


//------------------------------------------------
// Constructor
//------------------------------------------------
Scene::Scene(std::map<string,vector<Point>> &sFileNames)
{	
	m_pModels = new Model*[sFileNames.size()];
	m_pObjects = new Object*[sFileNames.size()];

	m_uiNumModels = sFileNames.size();
	std::map<string,vector<Point>>::iterator it = sFileNames.begin();

	for ( it=sFileNames.begin(); it != sFileNames.end(); ++it )
	{
		for(unsigned j = 0; j < (*it).second.size(); ++j)
		{
			++m_uiNumObjects;
		}
	}

	unsigned int i = 0;
	unsigned int objectCounter = 0;

	for ( it=sFileNames.begin(); it != sFileNames.end(); ++it, ++i )
	{
		m_pModels[i] = new Model((*it).first);
		for(unsigned j = 0; j < (*it).second.size(); ++j)
		{
			m_pObjects[objectCounter] = new Object(m_pModels[i], (*it).second[j]);
			++objectCounter;
		}
	}
}

//------------------------------------------------
// Destructor
//------------------------------------------------
Scene::~Scene()
{
	for(unsigned int i = 0; i < m_uiNumObjects; ++i)
	{
		SAFE_DELETE( m_pObjects[i] );
	}
	SAFE_DELETE( m_pObjects );
	
	for(unsigned int i = 0; i < m_uiNumModels; ++i)
	{
		SAFE_DELETE( m_pModels[i] );
	}
	SAFE_DELETE( m_pModels );
}

//------------------------------------------------
// Change structure for each model on the scene
//------------------------------------------------
void Scene::ChangeStructure(ACCELERATION_STRUCTURE eStructure)
{
	for(int i = m_uiNumModels-1; i >= 0; --i)
	{
		m_pModels[i]->SetCurrentStructureType(eStructure);
	}
}

//------------------------------------------------
// Rebuidl structure for each model on the scene
//------------------------------------------------
void Scene::Rebuild()
{
	for(int i = m_uiNumModels-1; i >= 0; --i)
	{
		m_pModels[i]->RebuildStructure(); 
	}
}