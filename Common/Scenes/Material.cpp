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

#include "Material.h"

Material::Material( unsigned int uiIndex, std::string sTextureName, MaterialData *pMaterialData )
{
	if(pMaterialData)
		m_pData = pMaterialData;
	else
		m_pData = new MaterialData();
	m_pTextureData = new MaterialTexture(sTextureName);
	m_uiIndex = uiIndex;
}

Material::~Material(void)
{
	if(m_pData) delete m_pData ;
	if(m_pTextureData) delete  m_pTextureData ;
}