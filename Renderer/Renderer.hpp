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

#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

namespace raytracer
{

enum RendererType
{
	RENDER_DX11,
	RENDER_OPENGL,
	RENDER_WINDOWS,
};

struct RendererUtils
{
	struct Config
	{
		
	}; 

	struct WindowConfig
	{
		UINT	m_Width;
		UINT	m_Height;
		HWND	m_hWnd;
	};

	RendererType m_eType;
};

class Renderer
{
public:
	typedef RendererUtils::WindowConfig WindowConfig;

	RendererType	m_eType;

	Renderer( RendererType eType ) : m_eType( eType ) {}

	virtual void*	GetWindow() const { return 0; }
	virtual void*	GetSwapChain() const { return 0; }

	virtual void	Initialize( const WindowConfig& wCfg ) {}
	virtual void	CreateViewport() {}
	virtual void	CreateBackBuffer() {}
	virtual void	TakeScreenshot() {}

};

};

#endif