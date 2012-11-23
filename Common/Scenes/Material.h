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

#ifndef __MATERIAL_H__
#define __MATERIAL_H__

//#include <string>
//#include "Texture2D.h"
//#include "Geometry.h"
#include "D3DResource.h"
#include "MaterialData.h"
#include "MaterialTextures.h"

class Material
{
private:
	MaterialData*		m_pData;
	MaterialTexture*	m_pTextureData;
	unsigned int		m_uiIndex; // id of the material
public:
	Material( unsigned int uiIndex, std::string sTextureName, MaterialData *pMaterialData = NULL );
	~Material( void );

	void				LoadTexture( std::string sPath, ID3D11Device* pDevice ) { m_pTextureData->LoadTexture(sPath,pDevice);}
	
	float*				GetDiffuse( void ) { return m_pData->GetDiffuse(); }
	unsigned int		GetIndex( void ) { return m_uiIndex; }
	std::string			GetTextureName( void ) { return m_pTextureData->GetTextureName(); }
	std::string			GetSpecularMap( void ) { return m_pTextureData->GetSpecularMap(); }
	std::string			GetNormalMap( void ) { return m_pTextureData->GetNormalMap(); }
	D3DResource*		GetTextureDiffuse() { return m_pTextureData->GetTextureDiffuse(); }
	D3DResource*		GetTextureSpecular() { return m_pTextureData->GetTextureSpecular(); }
	D3DResource*		GetTextureNormal() { return m_pTextureData->GetTextureNormal(); }
	MaterialData*		GetMaterialData() { return m_pData; }
	MaterialTexture*	GetTextureData() { return m_pTextureData; }

	void				SetDiffuse( const float *fDiffuse ) { m_pData->SetDiffuse(fDiffuse); }
	void				SetAmbient( const float *fAmbient ) { m_pData->SetAmbient(fAmbient); }
	void				SetSpecular( const float *fSpecular ) { m_pData->SetSpecular(fSpecular); }
	void				SetAlpha( const float fAlpha ) { m_pData->SetAlpha(fAlpha); }
	void				SetShininess( const float fShininess ) { m_pData->SetShininess(fShininess); }
};

#endif