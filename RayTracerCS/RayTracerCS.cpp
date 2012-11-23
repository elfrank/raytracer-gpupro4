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

#include "RayTracerCS.h"

RayTracerCS::RayTracerCS(Scene *a_Scene, HWND &m_hWnd)
{								

	m_csPrimaryRays = new D3DComputeShader();
	m_csIntersections = new D3DComputeShader();
	m_csColor = new D3DComputeShader();

	m_uRays = new D3DResource();
	m_uIntersections = new D3DResource();
	m_uAccumulation = new D3DResource();
	m_usPrimitives = new D3DResource();
	m_usNodes = new D3DResource();
	m_uResult = new D3DResource();

	// SRVs
	m_sVertices = new D3DResource();
	m_sIndices = new D3DResource();
	m_sPrimitivesData = new D3DResource();
	m_sColorTextures = new D3DResource();
	m_sNormalMapTextures = new D3DResource();
	m_sSpecularMapTextures = new D3DResource();
	m_sRandomMapTextures = new D3DResource();
	m_sEnvMapTextures = new D3DResource();
	m_sMaterialData = new D3DResource();
	m_uPrimitivesData = new D3DResource();

	m_cbCamera = new D3DResource();
	m_cbUserInput = new D3DResource();
	m_cbLight = new D3DResource();
	m_cbGlobalIllumination = new D3DResource();
	m_cbScene = new D3DResource();
	
	printf("Initializing raytracer...\n");

	/* get scene data */
	m_pScene = a_Scene;																									

	/* set number of groups to execute */
	GRID_SIZE[0] = 64, GRID_SIZE[1] = 64, GRID_SIZE[2] = 1;
	printf("Grid size: %d,%d,%d\n",GRID_SIZE[0],GRID_SIZE[1],GRID_SIZE[2]);

	/* initialize camera */
	m_pCamera = new Camera(Point(0.f,0.f,0.f), m_Parser.GetSpeed() );
	m_pCamera->SetCamera(1);
	
	/* initialize tracker */
	m_pTimeTracker = new Performance();

	/* initialize DirectX API for rendering */
	
	raytracer::Renderer::WindowConfig wConfig;
	raytracer::Device::Config config;

	config.m_eType = raytracer::DeviceUtils::Config::DEVICE_GPU;
	config.m_eVendor = raytracer::DeviceUtils::Config::VD_AMD;
	config.m_iDeviceIdx = 0;

	wConfig.m_Height = 1024;
	wConfig.m_Width = 1024;
	wConfig.m_hWnd = m_hWnd;

	m_pDeviceData = new raytracer::DeviceDX11;
	m_pRenderer = new raytracer::RendererDX11( (raytracer::DeviceDX11*)m_pDeviceData );

	m_pDeviceData->Initialize( config );
	m_pRenderer->Initialize( wConfig );
	m_pRenderer->CreateViewport();

	m_uResult->GetUAV()->SetView(((raytracer::RendererDX11*)m_pRenderer)->GetResource());

	// Initialize AntTweakBar
	/*if (!TwInit(TW_DIRECT3D11, ((ID3D11Device*)m_pDeviceData->GetDevice())))
    {
        MessageBoxA(m_hWnd, TwGetLastError(), "AntTweakBar initialization failed", MB_OK|MB_ICONERROR);
        //Cleanup();
        //return 0;
    }
	TwWindowSize(1024, 1024);

	// Create a tweak bar
    TwBar *bar = TwNewBar("TweakBar");
    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar into a DirectX11 application.' "); // Message added to the help bar.
    int barSize[2] = {224, 320};

	TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);

	bool a = false;
	TwAddVarRW(bar, "Animation", TW_TYPE_BOOLCPP, &a, "group=Sponge key=a");*/

	/* initialize light object (currently, it only support a single point light) */
	unsigned int iLightChoice = 1;
	m_pLight = new Light(iLightChoice);
	m_pInput = new Input();

	/* set the maximum number of ray bounces (initially defined at conf.ini) */
	m_pInput->SetNumBounces( m_Parser.GetNumReflections() );

	/* initialize shaders, materials and rendering */
	if(m_pScene)
	{
		Init();
	}

	/*HRESULT hr;

	// Compile the vertex shader
    ID3DBlob* pVSBlob = NULL;
	hr = m_pDXObject->CompileShaderFromFile( L"./RayTracerCS/Shaders/pixel.hlsl", "VS", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
       // return hr;
    }

	// Create the vertex shader
	hr = ((ID3D11Device*)m_pDeviceData->GetDevice())->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader );
	if( FAILED( hr ) )
	{	
		pVSBlob->Release();
       // return hr;
	}

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
	UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
	hr = ((ID3D11Device*)m_pDeviceData->GetDevice())->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
	pVSBlob->Release();

    // Set the input layout
	((ID3D11DeviceContext*)m_pDeviceData->GetContext())->IASetInputLayout( g_pVertexLayout );

	// Compile the pixel shader
    ID3DBlob* pPSBlob = NULL;
    hr = m_pDXObject->CompileShaderFromFile( L"./RayTracerCS/Shaders/pixel.hlsl", "PS", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        //return hr;
    }

    // Create the pixel shader
	hr = ((ID3D11Device*)m_pDeviceData->GetDevice())->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader );
    pPSBlob->Release();

	// Create vertex buffer
    SimpleVertex vertices[] =
    {
        XMFLOAT3( 0.0f, 0.5f, 0.5f ),
        XMFLOAT3( 0.5f, -0.5f, 0.5f ),
        XMFLOAT3( -0.5f, -0.5f, 0.5f ),
    };
    D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex ) * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
	hr = ((ID3D11Device*)m_pDeviceData->GetDevice())->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );

    // Set vertex buffer
    UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
	((ID3D11DeviceContext*)m_pDeviceData->GetContext())->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

    // Set primitive topology
    ((ID3D11DeviceContext*)m_pDeviceData->GetContext())->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	*/
	
	//TwAddVarRW(myBar, "NameOfMyVariable", TW_TYPE_xxxx, &myVar, "");
}

RayTracerCS::~RayTracerCS(void)
{
	/* delete data */
	SAFE_DELETE( m_pPrimitives );
	SAFE_DELETE( m_pTimeTracker );
	SAFE_DELETE( m_pInput );
	SAFE_DELETE( m_pCamera );
	SAFE_DELETE( m_pScene );

	SAFE_DELETE( m_csPrimaryRays );
	SAFE_DELETE( m_csIntersections );
	SAFE_DELETE( m_csColor );

	SAFE_DELETE( m_uRays );
	SAFE_DELETE( m_uIntersections );
	SAFE_DELETE( m_uAccumulation );
	SAFE_DELETE( m_usPrimitives );
	SAFE_DELETE( m_usNodes );
	SAFE_DELETE( m_uResult );
	SAFE_DELETE( m_uPrimitivesData );

	// SRVs
	SAFE_DELETE( m_sVertices );
	SAFE_DELETE( m_sIndices );
	SAFE_DELETE( m_sPrimitivesData );
	SAFE_DELETE( m_sColorTextures );
	SAFE_DELETE( m_sNormalMapTextures );
	SAFE_DELETE( m_sSpecularMapTextures );
	SAFE_DELETE( m_sRandomMapTextures );
	SAFE_DELETE( m_sEnvMapTextures );
	SAFE_DELETE( m_sMaterialData );

	SAFE_DELETE( m_cbCamera );
	SAFE_DELETE( m_cbUserInput );
	SAFE_DELETE( m_cbLight );
	SAFE_DELETE( m_cbGlobalIllumination );
	SAFE_DELETE( m_cbScene );

	SAFE_DELETE( m_pSConstruction );
	SAFE_DELETE( m_pRadixSort );
}

void RayTracerCS::Render()
{
	/* update current time */
	float fTimer = m_pTimeTracker->updateTime();
	/* Reset number of samples if the camera is moved */
	if (m_pCamera->Move( fTimer ))
	{
		m_NumMuestras = 0;
		m_cbGlobalIllumination->UpdateCB<cbGlobalIllumination, unsigned int>(&m_NumMuestras ,(ID3D11DeviceContext*)m_pDeviceData->GetContext());
	}

	/* Update constant buffers */
	m_cbCamera->UpdateCB<cbCamera,Camera>( m_pCamera, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );

	//m_usPrimitives->SetUAVs(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
	//m_usNodes->SetUAVs(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );

 	if( m_Parser.IsDynamic() )
	{
		/*m_usPrimitives->SetUAVs(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
		m_usNodes->SetUAVs(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );*/
		
		char* sName = m_pScene->GetModels()[0]->GetAccelStructure()->GetName();
		if ( sName == "LBVH" )
		{	
			m_pSConstruction->GPUBuildStructure( m_Parser.GetLBVHDepth(), ((ID3D11Device*)m_pDeviceData->GetDevice()), ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
		}

		/*m_usNodes->ClearUAV( ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
		m_usNodes->SetShaderResources(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
		m_usPrimitives->ClearUAV(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
		m_usPrimitives->SetShaderResources(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );*/

//		PostQuitMessage(0);
	}
	/*else
	{
		m_usNodes->ClearUAV(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
		m_usNodes->SetShaderResources(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
		m_usPrimitives->ClearUAV(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
		m_usPrimitives->SetShaderResources(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
	}*/

	/* Generate primary rays */

	m_cbGlobalIllumination->UpdateCB<cbGlobalIllumination, unsigned int>(&m_NumMuestras,  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
	m_csPrimaryRays->Dispatch( ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
	/* For each bounce, compute intersections and color */
	for(uint32_t r = 0; r < m_pInput->GetNumBounces()+1; r++)
	{
		/* Compute Intersections for each reflection */
		m_cbGlobalIllumination->UpdateCB<cbGlobalIllumination, unsigned int>(&m_NumMuestras,  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
		m_csIntersections->Dispatch( ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
		m_csColor->Dispatch( ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
	}

	//((ID3D11DeviceContext*)m_pDeviceData->GetContext())->VSSetShader(g_pVertexShader, NULL, 0);
	//((ID3D11DeviceContext*)m_pDeviceData->GetContext())->PSSetShader(g_pPixelShader, NULL, 0);
	//((ID3D11DeviceContext*)m_pDeviceData->GetContext())->Draw( 3, 0 );

	//TwDraw();
	
	/* Display frame on screen */
	((IDXGISwapChain*)m_pRenderer->GetSwapChain())->Present( 0, 0 );
	/* Update number of samples */
	++m_NumMuestras;
	Sleep(100);
	if ((m_NumMuestras%100)==0) {
		printf("Accumulated %d images\n",m_NumMuestras);
	}
}

HRESULT RayTracerCS::Init()
{
	HRESULT hr = S_OK;

	m_pSConstruction = new StructureConstruction( ((ID3D11Device*)m_pDeviceData->GetDevice()) );
	m_pRadixSort = new RadixSort( ((ID3D11Device*)m_pDeviceData->GetDevice()) );

	unsigned int uiNumPrimitives = m_pScene->GetModels()[0]->GetNumPrimitives();
	m_NumMuestras = 0;
	/* Fill Material Buffer for each primitive */
	m_pPrimitives = new PrimitiveCS[uiNumPrimitives];
	for(unsigned int i = 0; i < uiNumPrimitives; i++)
	{
		m_pPrimitives[i].uiMaterialId = m_pScene->GetModels()[0]->GetPrimitives()[i]->GetMaterial()->GetIndex();
	}

	/* Load the acceleration structure buffer */
	SelectAccelerationStructure();
	/* Load and compile the shaders */
	LoadShaders();
	/* Load diffuse textures, specular maps, normal maps and environment maps */
	LoadTextures();

	/* -------------------------------------------------------------
	 * UAVs and SRVs                
	 * ------------------------------------------------------------- */

	/* -------------------------------------------------------------
	 * SRVs
	 * ------------------------------------------------------------- */
	Model* pModel = m_pScene->GetModels()[0];

	raytracer::Buffer<Vertex> bVertices( m_pDeviceData, pModel->GetNumVertices() );
	raytracer::Buffer<DWORD> bIndices( m_pDeviceData, pModel->GetNumPrimitives()*3 );
	raytracer::Buffer<PrimitiveCS> bPrimitivesData( m_pDeviceData, pModel->GetNumPrimitives() );
	raytracer::Buffer<MaterialData> bMaterialsData( m_pDeviceData, pModel->GetNumMaterials() );
	
	bVertices.Write( pModel->GetVertices(), pModel->GetNumVertices() );
	bIndices.Write( pModel->GetIndices(), pModel->GetNumPrimitives()*3 );
	bPrimitivesData.Write( m_pPrimitives, pModel->GetNumPrimitives() );
	bMaterialsData.Write( pModel->GetMaterialsData(), pModel->GetNumMaterials() );

	//hr = m_sVertices->Init( SRV, STRUCTURED, &m_pScene->GetModels()[0]->GetVertices()[0], sizeof(Vertex),m_pScene->GetModels()[0]->GetNumVertices(), (ID3D11Device*)m_pDeviceData->GetDevice(), "sVertices" );
	//hr = m_sIndices->Init( SRV, STRUCTURED, &m_pScene->GetModels()[0]->GetIndices()[0], sizeof(DWORD),m_pScene->GetModels()[0]->GetNumPrimitives()*3, (ID3D11Device*)m_pDeviceData->GetDevice(), "sIndices" );
	//hr = m_sPrimitivesData->Init( SRV, STRUCTURED, &m_pPrimitives[0], sizeof(m_pPrimitives[0]),m_pScene->GetModels()[0]->GetNumPrimitives(), (ID3D11Device*)m_pDeviceData->GetDevice(), "sPrimitivesData" );
	//hr = m_sMaterialData->Init( SRV, STRUCTURED, &m_pScene->GetModels()[0]->GetMaterialsData()[0], sizeof(MaterialData),m_pScene->GetModels()[0]->GetNumMaterials(), (ID3D11Device*)m_pDeviceData->GetDevice(), "sMaterialsData" );
	
	m_sVertices->SetResource( bVertices.m_pPtr );
	m_sIndices->SetResource( bIndices.m_pPtr );
	m_sPrimitivesData->SetResource( bPrimitivesData.m_pPtr );
	m_sMaterialData->SetResource( bMaterialsData.m_pPtr );

	//m_vpSRViews.push_back(m_usPrimitives->GetSRV());
	//m_vpSRViews.push_back(m_usNodes->GetSRV());
	m_vpSRViews.push_back(m_sVertices->GetSRV());
	m_vpSRViews.push_back(m_sIndices->GetSRV());
	m_vpSRViews.push_back(m_sColorTextures->GetSRV());
	m_vpSRViews.push_back(m_sSpecularMapTextures->GetSRV());
	m_vpSRViews.push_back(m_sNormalMapTextures->GetSRV());
	m_vpSRViews.push_back(m_sPrimitivesData->GetSRV());
	m_vpSRViews.push_back(m_sRandomMapTextures->GetSRV());
	m_vpSRViews.push_back(m_sEnvMapTextures->GetSRV());
	m_vpSRViews.push_back(m_sMaterialData->GetSRV());
	m_vpSRViews.push_back(m_pSConstruction->GetSort()->GetResNumbersToOrder2()->GetSRV());

	/* copy resources from vector to array */
	ID3D11ShaderResourceView** pSRViews = new ID3D11ShaderResourceView*[m_vpSRViews.size()];
	for(unsigned int i = 0; i < m_vpSRViews.size(); i++)
	{
		m_vpSRViews[i]->SetId(i);
		m_vpSRViews[i]->SetLocked(true);
		pSRViews[i] = m_vpSRViews[i]->GetView();
	}

	pSRViews[0] = (ID3D11ShaderResourceView*) bVertices.m_pSRV;
	pSRViews[1] = (ID3D11ShaderResourceView*) bIndices.m_pSRV;
	pSRViews[5] = (ID3D11ShaderResourceView*) bPrimitivesData.m_pSRV;
	pSRViews[8] = (ID3D11ShaderResourceView*) bMaterialsData.m_pSRV;

	/* send shader resources to GPU */
	((ID3D11DeviceContext*)m_pDeviceData->GetContext())->CSSetShaderResources( 0, m_vpSRViews.size(), pSRViews );	
	
	/*-------------------------------------------------------------
	UAVs
	-------------------------------------------------------------*/

	/* Ray Tracing UAVs */
	raytracer::Buffer<Ray> bufRays( m_pDeviceData, WIDTH*HEIGHT );
	raytracer::Buffer<TIntersection> bufIntersections( m_pDeviceData, WIDTH*HEIGHT );
	//raytracer::Buffer<float4> bufAccumulation( m_pDeviceData, WIDTH*HEIGHT );

	m_uRays->SetResource( bufRays.m_pPtr );
	m_uIntersections->SetResource( bufIntersections.m_pPtr );
	
	//hr = m_uRays->Init(UAV, STRUCTURED, NULL, sizeof(Ray), WIDTH*HEIGHT, ((ID3D11Device*)m_pDeviceData->GetDevice()), "uRays");
	//hr = m_uIntersections->Init(UAV, STRUCTURED, NULL, sizeof(TIntersection), WIDTH*HEIGHT, ((ID3D11Device*)m_pDeviceData->GetDevice()), "uIntersections");
	hr = m_uAccumulation->Init(UAV, STRUCTURED, NULL, sizeof(Vector3)+sizeof(float), WIDTH*HEIGHT, ((ID3D11Device*)m_pDeviceData->GetDevice()), "uAccumulation");

	PrimitiveCS* prims = new PrimitiveCS[uiNumPrimitives];
	for(unsigned int i = 0; i < uiNumPrimitives; ++i)
	{
		prims[i].uiMaterialId = m_pScene->GetModels()[0]->GetPrimitives()[i]->GetMaterial()->GetIndex();
		prims[i].vfCentroid = Vector3(0.f,0.f,0.f);
		prims[i].vfMin = Vector3(-1.f,-1.f,-1.f);
		prims[i].vfMax = Vector3(1.f,1.f,1.f);
	}
	
	raytracer::Buffer<PrimitiveCS> bufPrimitivesData( m_pDeviceData, uiNumPrimitives );
	bufPrimitivesData.Write( prims, uiNumPrimitives );
	m_uPrimitivesData->SetResource( bufPrimitivesData.m_pPtr );

	//hr = m_uPrimitivesData->Init(SRV_AND_UAV, STRUCTURED, prims, sizeof(prims[0]), uiNumPrimitives, ((ID3D11Device*)m_pDeviceData->GetDevice()), "uPrimitives");

	/* SLBVH Construction */
	
	/*if( m_Parser.IsDynamic() )
	{
		m_usPrimitives->ClearSRV(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
		m_usNodes->ClearSRV(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
	}*/
	
	m_vpUAViews.push_back(m_uResult->GetUAV());
	m_vpUAViews.push_back(m_uRays->GetUAV());
	m_vpUAViews.push_back(m_uIntersections->GetUAV());
	m_vpUAViews.push_back(m_uAccumulation->GetUAV());
	m_vpUAViews.push_back(m_usPrimitives->GetUAV());
	m_vpUAViews.push_back(m_usNodes->GetUAV());
	m_vpUAViews.push_back(m_pSConstruction->GetSort()->GetResNumbersToOrder1()->GetUAV());
	m_vpUAViews.push_back(m_uPrimitivesData->GetUAV());

	/* copy resources from vector to array */
	ID3D11UnorderedAccessView** pUAViews = new ID3D11UnorderedAccessView*[m_vpUAViews.size()];
	for(unsigned int i = 0; i < m_vpUAViews.size(); i++)
	{
		m_vpUAViews[i]->SetId(i);
		m_vpUAViews[i]->SetLocked(true);
		pUAViews[i] = m_vpUAViews[i]->GetView();
	}

	pUAViews[1] = (ID3D11UnorderedAccessView*) bufRays.m_pUAV;
	pUAViews[2] = (ID3D11UnorderedAccessView*) bufIntersections.m_pUAV;
	pUAViews[7] = (ID3D11UnorderedAccessView*) bufPrimitivesData.m_pUAV;

	/*if( !m_Parser.IsDynamic() )
	{
		m_usPrimitives->ClearUAV(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
		m_usNodes->ClearUAV(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
	}*/

	((ID3D11DeviceContext*)m_pDeviceData->GetContext())->CSSetUnorderedAccessViews( 0, m_vpUAViews.size(), pUAViews, NULL );

	/*-------------------------------------------------------------
	CBs
	-------------------------------------------------------------*/
	
	raytracer::Buffer<cbCamera>			cbufferCamera( m_pDeviceData, 1, raytracer::BufferBase::BUFFER_CONST );
	raytracer::Buffer<cbInputOutput>	cbufferUserInput( m_pDeviceData, 1, raytracer::BufferBase::BUFFER_CONST );
	raytracer::Buffer<cbLight>			cbufferLight( m_pDeviceData, 1, raytracer::BufferBase::BUFFER_CONST );
	raytracer::Buffer<cbGlobalIllumination> cbufferGlobalIllum( m_pDeviceData, 1, raytracer::BufferBase::BUFFER_CONST );
	raytracer::Buffer<cbScene>			cbufferScene( m_pDeviceData, 1, raytracer::BufferBase::BUFFER_CONST );
	raytracer::Buffer<cbRadixSort>		cbufferRadixSort( m_pDeviceData, 1, raytracer::BufferBase::BUFFER_CONST );
	raytracer::Buffer<cbSLBVH>			cbufferSLBVH( m_pDeviceData, 1, raytracer::BufferBase::BUFFER_CONST );
	raytracer::Buffer<cbBBVH>			cbufferBBVH( m_pDeviceData, 1, raytracer::BufferBase::BUFFER_CONST );

	m_cbCamera->SetResource( cbufferCamera.m_pPtr );
	m_cbUserInput->SetResource( cbufferUserInput.m_pPtr );
	m_cbLight->SetResource( cbufferLight.m_pPtr );
	m_cbGlobalIllumination->SetResource( cbufferGlobalIllum.m_pPtr );
	m_cbScene->SetResource( cbufferScene.m_pPtr );
	m_pRadixSort->GetCBRadixSort()->SetResource( cbufferRadixSort.m_pPtr );
	m_pSConstruction->GetCBStructure()->SetResource( cbufferSLBVH.m_pPtr );
	m_pSConstruction->GetCBBBVH()->SetResource( cbufferBBVH.m_pPtr );

	m_vpCBuffers.push_back((ID3D11Buffer*) cbufferCamera.m_pPtr );
	m_vpCBuffers.push_back((ID3D11Buffer*) cbufferUserInput.m_pPtr );
	m_vpCBuffers.push_back((ID3D11Buffer*) cbufferLight.m_pPtr );
	m_vpCBuffers.push_back((ID3D11Buffer*) cbufferGlobalIllum.m_pPtr );
	m_vpCBuffers.push_back((ID3D11Buffer*) cbufferScene.m_pPtr );
	m_vpCBuffers.push_back((ID3D11Buffer*) m_pSConstruction->GetSort()->GetCBBitonicSort()->GetResource());
	m_vpCBuffers.push_back((ID3D11Buffer*) cbufferRadixSort.m_pPtr );
	m_vpCBuffers.push_back((ID3D11Buffer*) cbufferSLBVH.m_pPtr );
	m_vpCBuffers.push_back((ID3D11Buffer*) cbufferBBVH.m_pPtr );

	ID3D11Buffer** pCBuffers = new ID3D11Buffer*[m_vpCBuffers.size()];
	memcpy( pCBuffers, &m_vpCBuffers[0], sizeof( pCBuffers[0] ) * m_vpCBuffers.size() );
	((ID3D11DeviceContext*)m_pDeviceData->GetContext())->CSSetConstantBuffers(0, m_vpCBuffers.size(), pCBuffers);

	// Send data to constant buffers
	m_cbScene->UpdateCB<cbScene, Scene>( m_pScene, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
	m_cbCamera->UpdateCB<cbCamera, Camera>( m_pCamera, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
	m_cbUserInput->UpdateCB<cbInputOutput, Input>( m_pInput, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
	m_cbLight->UpdateCB<cbLight, Light>( m_pLight, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
	m_cbGlobalIllumination->UpdateCB<cbGlobalIllumination, unsigned int>( &m_NumMuestras, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );

	int LBVHData[] = { 1, m_Parser.GetLBVHDepth() };
	m_pSConstruction->GetCBStructure()->UpdateCB<cbSLBVH, int>( LBVHData, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );

	int BBVHData[] = { 1, 1, 1, 1 };
	m_pSConstruction->GetCBBBVH()->UpdateCB<cbBBVH, int>( BBVHData, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );

	return hr;
}

/*---------------------------------------------
** Loads geometry texture.
---------------------------------------------*/
HRESULT RayTracerCS::LoadTextures()
{
	HRESULT hr = S_OK;
	
	unsigned int uiNumMaterials = m_pScene->GetModels()[0]->GetNumMaterials();
          
	// Create buffers for textures
	hr = m_sColorTextures->Init(SRV, TEXTURE2D, NULL, 0, uiNumMaterials, ((ID3D11Device*)m_pDeviceData->GetDevice()), "sColorTextures");
	hr = m_sSpecularMapTextures->Init(SRV, TEXTURE2D, NULL, 0, uiNumMaterials, ((ID3D11Device*)m_pDeviceData->GetDevice()), "sSpecularMapTextures");
	hr = m_sNormalMapTextures->Init(SRV, TEXTURE2D, NULL, 0, uiNumMaterials, ((ID3D11Device*)m_pDeviceData->GetDevice()), "sNormalMapTextures");

	//raytracer::Buffer<float4> bufColorTx( m_pDeviceData, uiNumMaterials );
	
	//bufColorTx.Write( uiNumMaterials, NULL );
		
	//m_sColorTextures->SetResource( bufColorTx.m_pPtr );


	const string path = "./Models/Textures/";
	Material *pMaterial;
	for(unsigned int i = 0; i < uiNumMaterials; ++i)
	{	
		pMaterial = m_pScene->GetModels()[0]->GetMaterials()[i];
		if(pMaterial->GetTextureName().empty()) 
			continue;
		pMaterial->LoadTexture( path, ((ID3D11Device*)m_pDeviceData->GetDevice()) );
		((raytracer::DeviceDX11*)m_pDeviceData)->CreateTextureInArray((path+pMaterial->GetTextureName()).c_str(),pMaterial->GetTextureDiffuse()->GetResource(),m_sColorTextures->GetResource(),i);
		((raytracer::DeviceDX11*)m_pDeviceData)->CreateTextureInArray((path+pMaterial->GetSpecularMap()).c_str(),pMaterial->GetTextureSpecular()->GetResource(),m_sSpecularMapTextures->GetResource(),i);
		((raytracer::DeviceDX11*)m_pDeviceData)->CreateTextureInArray((path+pMaterial->GetNormalMap()).c_str(),pMaterial->GetTextureNormal()->GetResource(),m_sNormalMapTextures->GetResource(),i);
	}

	// Create Random Texture
	D3D11_TEXTURE2D_DESC dstex;
	ZeroMemory( &dstex, sizeof(dstex) );
    dstex.Width = 1024;
    dstex.Height = 1024;
    dstex.MipLevels = 1;
	dstex.ArraySize = 1;
    dstex.SampleDesc.Count = 1;
    dstex.SampleDesc.Quality = 0;
    dstex.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    dstex.Usage = D3D11_USAGE_DYNAMIC;
    dstex.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    dstex.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    dstex.MiscFlags = 0;
	
	hr = ((ID3D11Device*)m_pDeviceData->GetDevice())->CreateTexture2D( &dstex, NULL, (ID3D11Texture2D**)m_sRandomMapTextures->GetPtrResource() );
	
	// Load Random Texture
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
    ZeroMemory( &desc, sizeof(desc) );
    desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.MostDetailedMip = 0;
	desc.Texture2DArray.MipLevels = 1;
	desc.Texture2DArray.FirstArraySlice = 0;
	desc.Texture2DArray.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	hr = ((ID3D11Device*)m_pDeviceData->GetDevice())->CreateShaderResourceView( (ID3D11Resource*)m_sRandomMapTextures->GetResource(), &desc, m_sRandomMapTextures->GetSRV()->GetPtrView() );

	hr = ((raytracer::DeviceDX11*)m_pDeviceData)->CreateRandomTexture( (ID3D11Texture2D*)m_sRandomMapTextures->GetResource(), dstex.Width, dstex.Height );

	/* load environment map */
	const string name = path+"galileo_cross.dds";
	Texture2D* pEnvMap = new Texture2D();
	hr = pEnvMap->CreateEnvironmentMap(name.c_str(), ((ID3D11Device*)m_pDeviceData->GetDevice()));
	m_sEnvMapTextures->SetSRV(pEnvMap->GetTexture2D()->GetSRV());

	return hr;
}
/*-----------------------------------------------------------------------------*/
void RayTracerCS::SelectAccelerationStructure()
{
	/* get pointer to current acceleration structure */
	AccelerationStructure* pAStruct = m_pScene->GetModels()[0]->GetAccelStructure();

	bufPrimitives = raytracer::Buffer<unsigned int>( m_pDeviceData, m_pScene->GetModels()[0]->GetNumPrimitives() );

	if( m_Parser.IsDynamic() )
	{
		m_usPrimitives->Init(SRV_AND_UAV, STRUCTURED, NULL, sizeof(int), 
			m_pScene->GetModels()[0]->GetNumPrimitives(), ((ID3D11Device*)m_pDeviceData->GetDevice()), "uPrimitives");
		m_usNodes->Init(SRV_AND_UAV, STRUCTURED, NULL, 
			m_pScene->GetModels()[0]->GetAccelStructure()->GetSizeOfElement(), 
			m_pScene->GetModels()[0]->GetAccelStructure()->GetNumberOfElements(), 
			((ID3D11Device*)m_pDeviceData->GetDevice()), "uNodes");
	}
	else
	{
		//bufPrimitives.Write(  pAStruct->GetOrderedElementsIds(), m_pScene->GetModels()[0]->GetNumPrimitives() );

		m_usPrimitives->Init(SRV_AND_UAV, STRUCTURED, pAStruct->GetOrderedElementsIds(), 
			sizeof(pAStruct->GetOrderedElementsIds()[0]), 
			m_pScene->GetModels()[0]->GetNumPrimitives(), ((ID3D11Device*)m_pDeviceData->GetDevice()), "sPrimitives");
		m_usNodes->Init(SRV_AND_UAV, STRUCTURED, pAStruct->GetPtrToElements(), 
			pAStruct->GetSizeOfElement(), pAStruct->GetNumberOfElements(), 
			((ID3D11Device*)m_pDeviceData->GetDevice()), "sNodes");
	}

	//m_usPrimitives->SetResource( bufPrimitives.m_pPtr );
	
	/* set acceleration structure id sent to the GPU */
	m_pInput->SetAccelerationStructureFlag(pAStruct->GetKeyboardId());

	printf("Acceleration structure selected: %s\n", pAStruct->GetName());
}

/*-----------------------------------------------------------------------------*/
void RayTracerCS::ReloadAccelStructure()
{
	SelectAccelerationStructure();
	/* update buffers on GPU memory */
	/*m_usPrimitives->SetShaderResources(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
	m_usNodes->SetShaderResources(  ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );*/

	((ID3D11DeviceContext*)m_pDeviceData->GetContext())->CSSetUnorderedAccessViews( m_usPrimitives->GetUAV()->GetId(), 1, m_usPrimitives->GetUAV()->GetPtrView(), NULL );
	((ID3D11DeviceContext*)m_pDeviceData->GetContext())->CSSetUnorderedAccessViews( m_usNodes->GetUAV()->GetId(), 1, m_usNodes->GetUAV()->GetPtrView(), NULL );

	/* update input buffer to change current acceleration structure */
	m_cbUserInput->UpdateCB<cbInputOutput,Input>( m_pInput, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
}

//---------------------------------------------
// Load/Compile Vertex/Pixel/CS Shaders
//---------------------------------------------
void RayTracerCS::LoadShaders()
{
	// Compile and create compute shader
	unsigned int N = 0;
	std::vector<std::pair<string, int>> macros(4);
	macros[0] = std::pair<string,int>("BLOCK_SIZE_X",GROUP_SIZE[0]);
	macros[1] = std::pair<string,int>("BLOCK_SIZE_Y",GROUP_SIZE[1]);
	macros[2] = std::pair<string,int>("BLOCK_SIZE_Z",GROUP_SIZE[2]);
	macros[3] = std::pair<string,int>("N",N);

	/* number of groups per GPU */
	GRID_SIZE[0] = 64;
	GRID_SIZE[1] = 64;
	GRID_SIZE[2] = 1;

	/* number of threads per group */
	GROUP_SIZE[0] = 16;
	GROUP_SIZE[1] = 16;
	GROUP_SIZE[2] = 1;

	macros[0].second = GROUP_SIZE[0];	/* x-dimension */
	macros[1].second = GROUP_SIZE[1];	/* y-dimension */
	macros[2].second = GROUP_SIZE[2] ;	/* z-dimension */
	macros[3].second = GROUP_SIZE[0] * GRID_SIZE[0]; /* N */

	m_csPrimaryRays->Load((WCHAR*)L"./RayTracerCS/Shaders/App.hlsl",(LPCSTR)"CSGeneratePrimaryRays", ((ID3D11Device*)m_pDeviceData->GetDevice()), macros);
	m_csPrimaryRays->SetDimensiones(GRID_SIZE[0], GRID_SIZE[1], GRID_SIZE[2]);
	m_csIntersections->Load((WCHAR*)L"./RayTracerCS/Shaders/App.hlsl",(LPCSTR)"CSComputeIntersections", ((ID3D11Device*)m_pDeviceData->GetDevice()), macros);
	m_csIntersections->SetDimensiones(GRID_SIZE[0], GRID_SIZE[1], GRID_SIZE[2]);
	m_csColor->Load((WCHAR*)L"./RayTracerCS/Shaders/App.hlsl",(LPCSTR)"CSComputeColor",((ID3D11Device*)m_pDeviceData->GetDevice()), macros);
	m_csColor->SetDimensiones(GRID_SIZE[0], GRID_SIZE[1], GRID_SIZE[2]);
}

/*------------------------------------------------------------------------------------------------------
It handles the user input actions
------------------------------------------------------------------------------------------------------*/
LRESULT CALLBACK RayTracerCS::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:	m_pInput->SetMouseDown(true);	break;
	case WM_LBUTTONUP:		m_pInput->SetMouseDown(false);	break;
	case WM_MOUSEMOVE:
		if( m_pInput->IsMouseDown() )
		{
			UINT oldMouseX;
			UINT oldMouseY;
			m_pInput->OnMouseMove( oldMouseX, oldMouseY, lParam );
			m_pCamera->Rotate( oldMouseX,oldMouseY );
			m_NumMuestras = 0;
		}
		m_pInput->SetMouseCoordinates(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch ( wParam )
		{
		case 'W':			m_pCamera->SetFront(true); 		break;
		case 'A':			m_pCamera->SetLeft(true);		break;
		case 'S':			m_pCamera->SetBack(true);		break;
		case 'D':			m_pCamera->SetRight(true);		break;
		case 'Q':			m_pCamera->SetDown(true);		break;
		case 'E':			m_pCamera->SetUp(true);			break;
		case VK_ESCAPE:		printf("QUIT\n"); PostQuitMessage(0);				break;
		case VK_LEFT:		if(GetKeyState(VK_CONTROL) < 0){}
							else {
								m_pCamera->Turn(1,-22.5); 
								m_NumMuestras = 0;
							}
							break;
		case VK_RIGHT:		if(GetKeyState(VK_CONTROL) < 0){}
							else {
								m_pCamera->Turn(1,22.5);
								m_NumMuestras = 0;
							}
							break;
		case VK_DOWN:		if(GetKeyState(VK_CONTROL) < 0) m_pCamera->DecreaseSpeed();
							else {
								m_pCamera->Turn(0,-22.5);
								m_NumMuestras = 0;
							}
							break;
		case VK_UP:			if(GetKeyState(VK_CONTROL) < 0) m_pCamera->IncreaseSpeed();
							else {
								m_pCamera->Turn(0,22.5); 
								m_NumMuestras = 0;
							}
							break;
		case VK_HOME:		m_pCamera->Turn(2,22.5);
							m_NumMuestras = 0;
							break;
		case VK_END :		m_pCamera->Turn(2,-22.5);
							m_NumMuestras = 0;
							break;
		case 'P':			m_pCamera->ChangePausingState(); 
							m_cbUserInput->UpdateCB<cbInputOutput,Input>( m_pInput, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
							break;
		case 'F':			m_pInput->ChangePhongShadingState(); 
							m_cbUserInput->UpdateCB<cbInputOutput,Input>( m_pInput, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
							break;
		case 'K':			m_pInput->ChangeShadowingState(); 
							m_cbUserInput->UpdateCB<cbInputOutput,Input>( m_pInput, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
							break;
		case 'N':			m_pInput->ChangeNormalMappingState(); 
							m_cbUserInput->UpdateCB<cbInputOutput,Input>( m_pInput, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
							break;
		case 'M':			m_pInput->ChangeEnvMappingFlag(); 
							m_cbUserInput->UpdateCB<cbInputOutput,Input>( m_pInput, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
							break;
		case 'G':			m_pInput->ChangeGlossMappingState(); 
							m_cbUserInput->UpdateCB<cbInputOutput,Input>( m_pInput, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
							break;
		case 'T':			m_pInput->ChangeTexturingState(); 
							m_cbUserInput->UpdateCB<cbInputOutput,Input>( m_pInput, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
							break;
		case VK_SPACE:		m_pRenderer->TakeScreenshot(); 
							break;
		case VK_NUMPAD0:	
							if(GetKeyState(VK_CONTROL) < 0) 
							{
								m_pLight->SelectLight(0);
								m_cbLight->UpdateCB<cbLight,Light>( m_pLight, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
							} 
							else	{ m_pCamera->SetCamera(0); m_NumMuestras =0; }
							break;
		case VK_NUMPAD1:	
							if(GetKeyState(VK_CONTROL) < 0) 
							{ 
								m_pLight->SelectLight(1);
								m_cbLight->UpdateCB<cbLight,Light>( m_pLight, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
							} 
							else	{ m_pCamera->SetCamera(1); m_NumMuestras =0; }
							break;
		case VK_NUMPAD2:	m_pCamera->SetCamera(2); m_NumMuestras =0; break; /*sponza @lion*/
		case VK_NUMPAD3:	m_pCamera->SetCamera(3); m_NumMuestras =0; break;
		case VK_NUMPAD4:	m_pCamera->SetCamera(4); m_NumMuestras =0; break;
		case VK_NUMPAD5:	m_pCamera->SetCamera(5); m_NumMuestras =0; break;
		case VK_NUMPAD6:	m_pCamera->SetCamera(6); m_NumMuestras =0; break;
		case VK_NUMPAD7:	m_pCamera->SetCamera(7); m_NumMuestras =0; break;
		case VK_NUMPAD8:	m_pCamera->SetCamera(8); m_NumMuestras =0; break;
		case VK_NUMPAD9:	m_pCamera->SetCamera(9); m_NumMuestras =0; break;
		case VK_F4:			m_pCamera->ChangeOrbitingState(); break;
		}
		break;
	case WM_KEYUP:
		switch ( wParam )
		{
			case '1':	m_pInput->ChangeRebuildStructure(); 
						m_cbUserInput->UpdateCB<cbInputOutput,Input>( m_pInput, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
						break;
			case '0':	m_pScene->ChangeStructure(AS_BVH); 	ReloadAccelStructure(); break;
			case '9':	m_pScene->ChangeStructure(AS_LBVH); ReloadAccelStructure(); break;
			case '8':	m_pScene->ChangeStructure(AS_BLBVH); ReloadAccelStructure(); break;
			case 'W':	m_pCamera->SetFront(false);	break;
			case 'A':	m_pCamera->SetLeft(false);	break;
			case 'S':	m_pCamera->SetBack(false);	break;
			case 'D':	m_pCamera->SetRight(false);	break;
			case 'Q':	m_pCamera->SetDown(false);	break;
			case 'E':	m_pCamera->SetUp(false); break;
			case 'L':
				// Shaders
				m_csPrimaryRays->Release();
				m_csIntersections->Release();
				m_csColor->Release();
				LoadShaders();
				m_NumMuestras=0;
				break;
			case VK_ADD:		
				m_pInput->OperateOnBounces(+1);
				m_cbUserInput->UpdateCB<cbInputOutput,Input>( m_pInput, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
				break;
			case VK_SUBTRACT:	
				if (m_pInput->GetNumBounces() > 0)
					m_pInput->OperateOnBounces(-1);
				m_cbUserInput->UpdateCB<cbInputOutput,Input>( m_pInput, ((ID3D11DeviceContext*)m_pDeviceData->GetContext()) );
				break;
			case 'R':
				m_NumMuestras =0;
				break;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}