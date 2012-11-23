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

#include "D3DTexture2D.h"
#include "Common.h"

D3DTexture2D::D3DTexture2D(void)
{
}

D3DTexture2D::~D3DTexture2D(void)
{
}

HRESULT	D3DTexture2D::CreateTexture2D( UINT iNumElements, ID3D11Resource** ppResource, const char* pInitData, ID3D11Device* pDevice )
{
	HRESULT hr = S_OK;

	if(pInitData && fileExists(pInitData))
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		ZeroMemory( &loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO) );
		loadInfo.Width = 1024;
		loadInfo.Height = 1024;
		loadInfo.Depth = 1;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = 1;
		loadInfo.Usage = D3D11_USAGE_DEFAULT;
		loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		loadInfo.CpuAccessFlags = 0;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		loadInfo.Filter = D3DX11_DEFAULT;
		loadInfo.MipFilter = D3DX11_DEFAULT;
		loadInfo.pSrcInfo = NULL;  

		hr = D3DX11CreateTextureFromFileA( pDevice, /*reinterpret_cast<const char*>*/(pInitData), &loadInfo, NULL, ppResource, NULL );
	}
	else
	{
		D3D11_TEXTURE2D_DESC dstex;
		ZeroMemory( &dstex, sizeof(dstex) );
		dstex.Width = 1024;
		dstex.Height = 1024;
		dstex.MipLevels = 1;
		dstex.ArraySize = iNumElements;
		dstex.SampleDesc.Count = 1;
		dstex.SampleDesc.Quality = 0;
		dstex.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dstex.Usage = D3D11_USAGE_DEFAULT;
		dstex.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		dstex.CPUAccessFlags = 0;
		dstex.MiscFlags = 0;

		hr = pDevice->CreateTexture2D( &dstex, NULL, (ID3D11Texture2D**)ppResource );
	}

	if(FAILED(hr))
	{
		printf("ERROR: Failed to CreateTexture2D.\n");
	}

	return hr;

}

HRESULT	D3DTexture2D::CreateEnvironmentMap( LPCSTR sFileName, ID3D11Device* pDevice )
{
	HRESULT hr = S_OK;
    ID3D11Resource* pRes = NULL;

    D3DX11_IMAGE_LOAD_INFO LoadInfo;
    LoadInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = D3DX11CreateTextureFromFileA( pDevice, sFileName, &LoadInfo, NULL, &pRes, NULL );
    if( pRes )
    {
		printf("Create environment mapping %s\n", sFileName);
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory( &desc, sizeof( D3D11_TEXTURE2D_DESC ) );
        pRes->QueryInterface( __uuidof( ID3D11Texture2D ), ( LPVOID* )&m_pTexture );
        m_pTexture->GetDesc( &desc );
        SAFE_RELEASE( pRes );

        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
        ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
        SRVDesc.Format = desc.Format;
        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        SRVDesc.TextureCube.MipLevels = desc.MipLevels;
        SRVDesc.TextureCube.MostDetailedMip = 0;
		hr = pDevice->CreateShaderResourceView( m_pTexture, &SRVDesc, &m_pSRV );
    }

	return hr;
}
