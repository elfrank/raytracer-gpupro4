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

#include "MaterialTextures.h"

MaterialTexture::~MaterialTexture(void)
{
}

//-------------------------------------------------------------------------
// Get the specular map file name
//-------------------------------------------------------------------------
std::string MaterialTexture::GetSpecularMap( void ) 
{ 
	std::string extension = m_sTextureName.substr(m_sTextureName.size()-4, 4);
	std::string diff = m_sTextureName.substr(m_sTextureName.size()-8, 4);
	if(diff == "diff")
	{
		return m_sTextureName.substr(0, m_sTextureName.size()-8) + "spec" + extension;
	}
	else
	{
		return m_sTextureName.substr(0, m_sTextureName.size()-4) + "_spec" + extension;
	}
}

//-------------------------------------------------------------------------
// Get the normal map file name
//-------------------------------------------------------------------------
std::string MaterialTexture::GetNormalMap( void ) 
{ 
	std::string extension = m_sTextureName.substr(m_sTextureName.size()-4, 4);
	std::string diff = m_sTextureName.substr(m_sTextureName.size()-8, 4);
	if(diff == "diff")
	{
		return m_sTextureName.substr(0, m_sTextureName.size()-8) + "ddn" + extension;
	}
	else
	{
		return m_sTextureName.substr(0, m_sTextureName.size()-4) + "_ddn" + extension;
	}
}

void MaterialTexture::LoadTexture( std::string sPath, ID3D11Device* pDevice )
{
	if(!m_sTextureName.empty()) 
	{
		m_pTextureDiffuse.Init(SRV, TEXTURE2D, (void*)((sPath + m_sTextureName).c_str()), 0, 1, pDevice);
		m_pTextureSpecularMap.Init(SRV, TEXTURE2D, (void*)((sPath + GetSpecularMap()).c_str()), 0, 1, pDevice);
		m_pTextureNormalMap.Init(SRV, TEXTURE2D, (void*)((sPath + GetNormalMap()).c_str()), 0, 1, pDevice);
	}
	
}
