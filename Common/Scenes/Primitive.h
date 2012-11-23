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

#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include "Material.h"
#include "Geometry.h"
#include "Vertex.h"

class Primitive
{
private:
	Material*				m_pMaterial;
	unsigned				int m_uiType;

	union
	{
		// Triangle
		struct
		{
			Vertex*			m_pVertex[3];
			//Vector		m_Normal;
		};
	};
public:
	enum
	{
		TRIANGLE = 1,
	};
	Primitive(Vertex* pVertex1, Vertex* pVertex2, Vertex* pVertex3);
	~Primitive(void);

	unsigned int			GetType() { return m_uiType; }
	
	int						IntersectP(Ray &a_Ray, float &a_T);
	Color					GetColor( void ) { }

	Material*				GetMaterial( void ) { return m_pMaterial; }
	Vertex*					GetVertex( unsigned int uiIdx ) { return m_pVertex[uiIdx]; }

	void					SetMaterial( Material* pMaterial ) { m_pMaterial = pMaterial; }
	void					SetVertex( unsigned int uiIdx, Vertex* pVertex ) { m_pVertex[uiIdx] = pVertex; }
	void					PrintString();
};

#endif