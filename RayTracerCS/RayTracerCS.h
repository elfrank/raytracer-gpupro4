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

#ifndef __RAYTRACERCS_H__
#define __RAYTRACERCS_H__

#include <limits>

#include <ArgumentsParser.h>
extern ArgumentsParser m_Parser;

#include <DeviceCore/DeviceCore.hpp>
#include <Renderer/RendererCore.hpp>

#include <Common/Common.h>
#include <Common/Scenes/Scene.h>
#include <Common/Camera.h>
#include <Common/Input.h>
#include <Common/Performance.h>

#include <RayTracerCS/StructureConstruction.h>
#include <RayTracerCS/RadixSort.h>
#include <RayTracerCS/ConstantBuffers.h>

#include <RayTracerCS/Texture2D.h>

//#include <AntTweakBar.h>
#include <xnamath.h>

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
    XMFLOAT3 Pos;
};

struct PrimitiveCS
{
	Vector3 vfCentroid;			/* 4 bytes */
	Vector3 vfMin;				/* 12 bytes */
	Vector3 vfMax;				/* 12 bytes */
	int uiMaterialId;			/* 4 bytes */
	int uiId;					/* 4 bytes */

	void PrintString()
	{
		//printf("Centroid(%f,%f,%f), BoxMin(%f,%f,%f), BoxMax(%f,%f,%f)\n", vfCentroid.x,vfCentroid.y,vfCentroid.z,vfMin.x,vfMin.y,vfMin.z,vfMax.x,vfMax.y,vfMax.z);
		printf("BoxMin(%f,%f,%f), BoxMax(%f,%f,%f)\n", vfMin.x,vfMin.y,vfMin.z,vfMax.x,vfMax.y,vfMax.z);
	}
};

class RayTracerCS
{
public:
	RayTracerCS(Scene *a_Scene, HWND &m_hWnd);
	~RayTracerCS();

	void									Render();
	LRESULT CALLBACK						WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool									UpdateWindowTitle(char aux[1024], char* str) { return m_pTimeTracker->updateFPS(aux,str);}

private:
	ID3D11PixelShader* g_pPixelShader;
	ID3D11VertexShader* g_pVertexShader;
	ID3D11InputLayout*      g_pVertexLayout;
	ID3D11Buffer*           g_pVertexBuffer;


	D3DResource*							m_usPrimitives; // empty
	D3DResource*							m_usNodes; // empty

	/* UAVs */
	D3DResource*							m_uRays; // store the generated/bounced rays
	D3DResource*							m_uIntersections; // store the intersection information for a given ray
	D3DResource*							m_uAccumulation; // store the accumulated color
	D3DResource*							m_uResult; // store the texture to render
	D3DResource*							m_uPrimitivesData;

	/* SRVs */
	D3DResource*							m_sVertices; // store the vertices of the model
	D3DResource*							m_sIndices;	// store the indices of the model
	D3DResource*							m_sPrimitivesData; // store the material id of a given primitive
	D3DResource*							m_sColorTextures; // store an Array2D of textures
	D3DResource*							m_sNormalMapTextures; // store an Array2D of normal maps
	D3DResource*							m_sSpecularMapTextures; // store an Array2D of sepecular maps
	D3DResource*							m_sRandomMapTextures; // store random values for GI app
	D3DResource*							m_sEnvMapTextures; // store an environment mapping texture
	D3DResource*							m_sMaterialData;

	/* Constant Buffers */
	D3DResource*							m_cbCamera; // store the camera information
	D3DResource*							m_cbUserInput; // store the values of current keyboard/mouse status
	D3DResource*							m_cbLight; // store the light information
	D3DResource*							m_cbGlobalIllumination; // store the GI values
	D3DResource*							m_cbScene;

	/* Compute shaders */
	D3DComputeShader*						m_csPrimaryRays; // generate the primary rays
	D3DComputeShader*						m_csIntersections; // compute ray-triangle intersections
	D3DComputeShader*						m_csColor; // compute the color of a pixel
	
	UINT									GRID_SIZE[3]; // number of groups to execute (xyz)
	UINT									GROUP_SIZE[3]; // number of threads per group (xyz)
	std::vector<D3DView<ID3D11UnorderedAccessView>*>	m_vpUAViews; // store the UAVs structures used by the CSs
	std::vector<D3DView<ID3D11ShaderResourceView>*>	m_vpSRViews; // store the SRVs structures used by the CSs
	std::vector<ID3D11Buffer*>				m_vpCBuffers; // store the CBs used by the CSs

	// Objects
	raytracer::Device*						m_pDeviceData; // Device object
	raytracer::Renderer*					m_pRenderer; // Renderer object
	Scene*									m_pScene; // loaded scene information
	Camera*									m_pCamera; // camera definition
	Light*									m_pLight; // light object
	Input*									m_pInput; // user input (keyboard-mouse) object
	PrimitiveCS*							m_pPrimitives; 
	Performance*							m_pTimeTracker; // time tracking helper class
	StructureConstruction*					m_pSConstruction;
	RadixSort*								m_pRadixSort;
	
	
	unsigned  int							m_NumMuestras;

	raytracer::Buffer<unsigned int> bufPrimitives;//( m_pDeviceData, m_pScene->GetModels()[0]->GetNumPrimitives() );

	// Functions
	HRESULT									CreateBVHBuffers();
	HRESULT									Init();
	HRESULT									LoadTextures();
	void									LoadShaders();
	void									SelectAccelerationStructure();
	void									ReloadAccelStructure();
};

//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
/*HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}*/

#endif