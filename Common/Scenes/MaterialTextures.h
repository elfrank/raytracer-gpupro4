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

#ifndef __MATERIALTEXTURE_H__
#define __MATERIALTEXTURE_H__


#include <string>
#include "Texture2D.h"
#include "D3DResource.h"

class MaterialTexture
{
private:
	std::string		m_sTextureName;
	D3DResource		m_pTextureDiffuse; 
	D3DResource		m_pTextureSpecularMap; 
	D3DResource		m_pTextureNormalMap;
public:
	MaterialTexture( std::string sTextureName ) : m_sTextureName(sTextureName) {}
	~MaterialTexture(void);

	void			LoadTexture( std::string sPath, ID3D11Device* pDevice );

	std::string		GetTextureName( void ) { return m_sTextureName; }
	std::string		GetSpecularMap( void );
	std::string		GetNormalMap( void );
	D3DResource*	GetTextureDiffuse() { return &m_pTextureDiffuse; }
	D3DResource*	GetTextureSpecular() { return &m_pTextureSpecularMap; }
	D3DResource*	GetTextureNormal() { return &m_pTextureNormalMap; }

};

#endif