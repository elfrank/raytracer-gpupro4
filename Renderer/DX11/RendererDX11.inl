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

#include <D3DX11tex.h>
#include <D3DX11.h>
#include <D3DX11core.h>
#include <D3DX11async.h>
#include <D3Dcompiler.h>

#include <string>
#include <iostream>
#include <sstream>

namespace raytracer
{

class RendererDX11: public Renderer
{
public:
	typedef RendererUtils::Config Config;

	WindowConfig m_WindowConfig;
	DeviceDX11* m_pDevice;

	IDXGISwapChain* m_pSwapChain;
	ID3D11Texture2D* m_pBackBufferTexture;
	ID3D11RenderTargetView*	m_pRenderTargetView;
	ID3D11UnorderedAccessView* m_ppResultUAV;
	ID3D11SamplerState*			m_pSamplerLinear;

	__inline RendererDX11( DeviceDX11* pDevice ) : Renderer( RENDER_DX11 ), m_pDevice( pDevice ) {}
	__inline ~RendererDX11();

	__inline void Initialize( const WindowConfig& wCfg );
	__inline void CreateViewport();
	__inline void CreateBackBuffer();
	__inline void TakeScreenshot();
	__inline void CreateSamplerStates();

	__inline ID3D11UnorderedAccessView* GetResource() { return m_ppResultUAV; }
	__inline void* GetSwapChain() const { return m_pSwapChain; }
};

RendererDX11::~RendererDX11()
{

}

void RendererDX11::Initialize( const WindowConfig& wCfg )
{
	ID3D11Device* pDevice = ((ID3D11Device*)m_pDevice->GetDevice());
	m_WindowConfig = wCfg;
	
	HRESULT hr;

	IDXGIDevice * pDXGIDevice;
	hr = pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);
	
	IDXGIFactory * pIDXGIFactory;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);
	
	DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = m_WindowConfig.m_Width;
    sd.BufferDesc.Height = m_WindowConfig.m_Height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
    sd.OutputWindow = m_WindowConfig.m_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	hr = pIDXGIFactory->CreateSwapChain( pDevice, &sd, &m_pSwapChain );
	hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>(&m_pBackBufferTexture) );

	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
	ZeroMemory( &DescUAV, sizeof(DescUAV) );
	DescUAV.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	DescUAV.Texture2D.MipSlice=0;

	hr = pDevice->CreateUnorderedAccessView( m_pBackBufferTexture, &DescUAV, &m_ppResultUAV );

	hr = pDevice->CreateRenderTargetView( m_pBackBufferTexture, NULL, &m_pRenderTargetView );

	((ID3D11DeviceContext*)m_pDevice->GetContext())->OMSetRenderTargets(1,&m_pRenderTargetView,NULL);

	CreateSamplerStates();
}

void RendererDX11::CreateViewport()
{
	printf("Create viewport\n");

	ID3D11DeviceContext* pContext = ((ID3D11DeviceContext*)m_pDevice->GetContext());

	RECT rc;
	GetClientRect( m_WindowConfig.m_hWnd, &rc );
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

    D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    pContext->RSSetViewports( 1, &vp );
}

void RendererDX11::CreateBackBuffer()
{

}

void RendererDX11::TakeScreenshot()
{
	HRESULT hr = S_OK;
	
	std::string str;
    LPWSTR str2 = L".\\RayTracerCS\\Screenshots\\test.png";

	cvtLPW2stdstring(str,str2);
	hr = D3DX11SaveTextureToFile(((ID3D11DeviceContext*)m_pDevice->GetContext()),m_pBackBufferTexture,D3DX11_IFF_PNG,str2);

	if(FAILED(hr))
	{
		printf("Screenshot FAILED\n");
		//return hr;
	}
	printf("Screenshot saved at %s\n",str.c_str());
}

void RendererDX11::CreateSamplerStates()
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(D3D11_SAMPLER_DESC) );
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	hr = ((ID3D11Device*)m_pDevice->GetDevice())->CreateSamplerState( &sampDesc, &m_pSamplerLinear );
	((ID3D11DeviceContext*)m_pDevice->GetContext())->CSSetSamplers( 0, 1, &m_pSamplerLinear );
}

};