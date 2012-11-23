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

#ifndef _MODEL_H_
#define _MODEL_H_

#include "string.h"
#include "Common.h"
#include "Triangle.h"
#include "Vertex.h"

#include "NullShader.h"
#include "bvh.h"
#include "CubicGrid.h"
#include "Simple.h"
#include "BIH.h"
#include "LBVH.h"
#include "KDTree.h"

#include <stdio.h>
#include "Loaders/model_obj.h"
#include <list>

class Model
{
private:
	Primitive					**m_ppPrimitives; // pointer to the list of primitives
	Material					**m_ppMaterials; // pointer to the list of materials
	MaterialData				*m_ppMaterialsData; 
	AccelerationStructure		*m_pAccelStructure; // pointer to the current AS
	Vertex						*m_pVertices; // pointer to vertex array
	DWORD						*m_pIndices; // 3 indices = 1 triangle
	unsigned int				m_uiNumVertices; // total number of vertices
	unsigned int				m_uiNumPrimitives; // total number of primitives
	unsigned int				m_uiNumMaterials; // total number of materials
	string						m_sName; // name of the model
	ACCELERATION_STRUCTURE		m_eAccelStructureType;  // acceleration structure built type
	ModelOBJ					m_rModelObj; // *.obj model loader
public:
	Model( string sFileName = NULL );
	~Model(void);

	Primitive**					GetPrimitives( void ) { return m_ppPrimitives; }
	AccelerationStructure*		GetAccelStructure( void ) { return m_pAccelStructure; }
	Vertex*						GetVertices( void ) { return m_pVertices; }
	Material**					GetMaterials( void ) { return m_ppMaterials; }
	MaterialData*				GetMaterialsData( void ) { return m_ppMaterialsData; } 
	string						GetName( void ) { return m_sName; }
	unsigned int				GetNumPrimitives() { return m_uiNumPrimitives; }
	unsigned int				GetNumVertices() { return m_uiNumVertices; }
	unsigned int				GetNumMaterials() { return m_uiNumMaterials; }
	DWORD*						GetIndices() { return m_pIndices; }
	BBox						GetBounds() { return m_pAccelStructure->WorldBound(); }
	void						RebuildStructure();
	void						SetCurrentStructureType(ACCELERATION_STRUCTURE eStructure);
	void						LoadFile(string a_FileName);
	void						Scale();
	// The actual threshold is: fThreshold * MaxArea. Anything above this value is tessellated recursively.
	void						Tessellate(float fThreshold);
	float						GetPrimitiveArea(unsigned int iNumPrimitive);
	void						RecTessellate(list<Vertex*> *inout_VList, list<Material*> *inout_MList, Vertex v0, Vertex v1, Vertex v2, Material *M, float fThreshold);
};

#endif