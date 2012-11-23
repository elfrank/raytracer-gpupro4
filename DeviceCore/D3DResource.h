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

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <D3DX11tex.h>
#include <D3DX11.h>
#include <D3DX11core.h>
#include <D3DX11async.h>
#include <D3Dcompiler.h>

#include "D3DTexture2D.h"
#include <DeviceCore/DeviceCore.hpp>

enum BufferType { RAW, STRUCTURED, TEXTURE2D };	// different types of buffer
enum BufferBind { SRV, UAV, SRV_AND_UAV, CONSTANT }; // different types of binding for a resource

template <class Type>
class D3DView
{
private:
	Type*			m_pView;
	int				m_uiId;
	bool			m_bLocked;
public:
	D3DView() 
	{
		m_pView = NULL;
		m_uiId = -1;
	}

	~D3DView() 
	{
		SAFE_RELEASE( m_pView );
	}

	unsigned int	GetId() { return m_uiId; }
	Type*			GetView() { return m_pView; }
	Type**			GetPtrView() { return &m_pView; }
	bool			IsLocked() { return m_bLocked; }

	void			SetId(unsigned int uiId) { m_uiId = uiId; }
	void			SetView(Type* pView) { m_pView = pView; }
	void			SetLocked( bool bLocked ) { m_bLocked = bLocked; }
};

class D3DResource
{
public:
	D3DResource();
	~D3DResource();

	template <class R, class T>  HRESULT		UpdateCB( T* pObj, ID3D11DeviceContext* pContext )
	{
		HRESULT hr;

		D3D11_MAPPED_SUBRESOURCE mp; 
		hr = pContext->Map( m_pResource,0,D3D11_MAP_WRITE_DISCARD,0,&mp );
		if( FAILED( hr ) )
		{
			printf("FAILED updating buffer.\n");
			return hr;
		}
		R* tmp = (R*)mp.pData;
		tmp->Update( pObj );
		pContext->Unmap( m_pResource,0 );

		return S_OK;
	}

	/*------------------------------------------------------------------------------------------------------
	** Prints the buffer content in the output window.
	**------------------------------------------------------------------------------------------------------*/
	template <class U>
	HRESULT DebugCSBuffer ( int iEnd, int iStart, ID3D11DeviceContext* pContext, raytracer::DeviceDX11* pDeviceData ) 
	{
		HRESULT hr = S_OK;
		
		/* create read-write buffer on CPU side */
		ID3D11Buffer* debugBuf = pDeviceData->CreateAndCopyToDebugBuf( static_cast<ID3D11Buffer*>(m_pResource) );
		/* copy data to CPU memory */
		D3D11_MAPPED_SUBRESOURCE MappedResource; 
		hr = pContext->Map( debugBuf, 0, D3D11_MAP_READ, 0, &MappedResource );
		if(FAILED(hr))
		{
			printf("FAILED debug function on resource mapping.\n");
			return hr;
		}
		
		/* cast buffer to a user defined class */
		U* p = static_cast<U*>(MappedResource.pData);
		
		FILE *fp;
		if( !( fp = freopen( "OUT.txt", "w" ,stdout ) )) 
		{
			printf("Cannot open file.\n");
			exit(1);
		}

		/* print content of the buffer */
		printf("\n-----------------------------------\nDEBUG %s\n-----------------------------------\n", typeid(U).name());
		for(int i = iStart; i < iEnd; ++i)
		{
			printf("{%d}: ",i);
			p[i].PrintString();
			//printf("[%d]:%d",i,p[i]);
			//printf("\n");	
		}

		fclose(fp);

		/* clean context */
		pContext->Unmap( debugBuf, 0 );
		debugBuf->Release();	

		return hr;
	}

	HRESULT Init( BufferBind a_iBind, BufferType a_iType, VOID* a_InitData, size_t a_SizeInBytes, 
		UINT a_iNumElements, ID3D11Device* pDevice, char* pcStr = "unnamed" );
	HRESULT CreateUAV( ID3D11Device* pDevice );
	HRESULT CreateSRV( ID3D11Device* pDevice );
	

	ID3D11Resource*				GetResource() { return m_pResource; }
	ID3D11Resource**			GetPtrResource() { return &m_pResource; }
	D3DView<ID3D11UnorderedAccessView>*					GetUAV() { return m_pUAV; }
	D3DView<ID3D11ShaderResourceView>*					GetSRV() { return m_pSRV; }

	void				SetResource(void* pRes) { m_pResource = (ID3D11Resource*)pRes; }

	void SetShaderResources( ID3D11DeviceContext* pContext, int iId = -1 ) 
	{ 
		int id = iId == (-1) ? m_pSRV->GetId() : iId;
		ID3D11ShaderResourceView* pSRViewNULL = NULL;
        
		if( (m_pUAV->GetId() >= 0) && (m_pUAV->IsLocked()) )
		{
			m_pUAV->SetLocked(false);
			ID3D11UnorderedAccessView* pUAViewNULL = NULL;
			//pDXObject->GetDeviceContext()->CSSetUnorderedAccessViews( m_pUAV->GetId(), 1, &pUAViewNULL, NULL );
		}

		pContext->CSSetShaderResources( id, 1, &pSRViewNULL );
		pContext->CSSetShaderResources( 
			id, 1, m_pSRV->GetPtrView() );
		m_pSRV->SetLocked(true);
	}

	void SetUAVs( ID3D11DeviceContext* pContext, int iId = -1 ) 
	{ 
		int id = iId == (-1) ? m_pUAV->GetId() : iId;
		ID3D11UnorderedAccessView* pUAViewNULL = NULL;
		
		if( (m_pSRV->GetId() >= 0) && (m_pSRV->IsLocked()) )
		{
			m_pSRV->SetLocked(false);
			ID3D11ShaderResourceView* pSRViewNULL = NULL;
			pContext->CSSetShaderResources( m_pSRV->GetId(), 1, &pSRViewNULL );
		}
		
		pContext->CSSetUnorderedAccessViews( id, 1, &pUAViewNULL, NULL );
		pContext->CSSetUnorderedAccessViews( 
			id, 1, m_pUAV->GetPtrView(), NULL );
		m_pUAV->SetLocked(true);
	}

	void ClearUAV( ID3D11DeviceContext* pContext, int iId = -1 )
	{
		int id = iId == (-1) ? m_pUAV->GetId() : iId;
		ID3D11UnorderedAccessView* pUAViewNULL = NULL;
		pContext->CSSetUnorderedAccessViews( id, 1, &pUAViewNULL, NULL );
	}

	void ClearSRV( ID3D11DeviceContext* pContext, int iId = -1 )
	{
		int id = iId == (-1) ? m_pSRV->GetId() : iId;
		ID3D11ShaderResourceView* pSRViewNULL = NULL;
		pContext->CSSetShaderResources( id, 1, &pSRViewNULL );
	}

	void						SetSRV(ID3D11ShaderResourceView* pSRV) { m_pSRV->SetView(pSRV); }
private:
	ID3D11Resource*				m_pResource;
	D3DView<ID3D11UnorderedAccessView>* m_pUAV;
	D3DView<ID3D11ShaderResourceView>* m_pSRV;
	char						m_cName[256];
	size_t						m_tSize;
	size_t						m_tNumOfElements;
	BufferType					m_eBufferType;
	BufferBind					m_eBufferBind;

	void						StoreName( char* pcStr, ID3D11Resource* pObject );
	void						GetName( char c_szName[] );
	void						PrintName();
	void						Print();
};

#endif