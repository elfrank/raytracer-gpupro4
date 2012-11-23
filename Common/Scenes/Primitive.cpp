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

#include "Primitive.h"

Primitive::Primitive(Vertex* pVertex1, Vertex* pVertex2, Vertex* pVertex3)
{
	m_pVertex[0] = pVertex1;
	m_pVertex[1] = pVertex2;
	m_pVertex[2] = pVertex3;
}

Primitive::~Primitive(void)
{

}

void Primitive::PrintString()
{
	printf("V[0]-> ");
	m_pVertex[0]->PrintString();
	printf("\nV[1]-> ");
	m_pVertex[1]->PrintString();
	printf("\nV[2]-> ");
	m_pVertex[2]->PrintString();
	printf("\n");
}