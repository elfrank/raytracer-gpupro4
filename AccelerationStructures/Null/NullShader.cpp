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

#include "NullShader.h"

NullShader::NullShader(Primitive** pPrimitives, unsigned int uiNumPrimitives)
{
	m_sName = "NullStructure";
	m_pPrimitives = pPrimitives;
	m_uiNumPrimitives = uiNumPrimitives;
}

NullShader::~NullShader()
{

}

TIntersection NullShader::IntersectP(Ray &a_Ray)
{
	TIntersection res;
	return res;
}

void NullShader::PrintOutput(float &fTime)
{
	printf("Tiempo en generar estructuras: %f segundos\n",fTime);
}