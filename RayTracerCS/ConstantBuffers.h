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

#include "Camera.h"
#include "Light.h"

// Constant buffer structures for the GPU.
// * They must implement an Update function.
// * Its size must be a multiple of 32 bytes or the app will crash.

// Camera
struct cbCamera
{
	//Multiple of 32 bytes
	Matrix4	mWorldMatrix;				// 64 bytes - World matrix

	void Update( Camera* pCamera )
	{
		mWorldMatrix = pCamera->GetInverseMatrix();
	}
};

// User input-output
struct cbInputOutput
{
	//Multiple of 32 bytes
	int		bIsShadowOn;				// 4 bytes
	int		bIsPhongShadingOn;			// 4 bytes
	int		bIsNormalMappingOn;			// 4 bytes
	int		bIsGlossMappingOn;			// 4 bytes
	int		iAccelerationStructure;		// 4 bytes
	int		iEnvMapping;				// 4 bytes
	UINT	iNumBounces;				// 4 bytes
	int		bIsRebuildStructureOn;		// 4 bytes
	int		bIsTexturingOn;				// 4 bytes
	int		bIOPadding[7];				// 28 bytes

	cbInputOutput()
	{
		bIsShadowOn	= 1;
		bIsPhongShadingOn = 0;
		bIsNormalMappingOn = 1;
		bIsGlossMappingOn = 1;
		iEnvMapping = -1;
		iNumBounces = 0;
		bIsTexturingOn = 1;
	}
	
	void Update( Input* pInput )
	{
		bIsShadowOn = static_cast<int>(pInput->IsShadow());
		bIsPhongShadingOn = static_cast<int>(pInput->IsFlatShading());
		iEnvMapping = static_cast<int>(pInput->GetEnvMappingFlag());
		bIsNormalMappingOn = static_cast<int>(pInput->IsNormalMappingOn());
		bIsGlossMappingOn = static_cast<int>(pInput->IsGlossMappingOn());
		iAccelerationStructure = static_cast<int>(pInput->GetAccelerationStructureFlag());
		iNumBounces = static_cast<int>(pInput->GetNumBounces());
		bIsTexturingOn = static_cast<int>(pInput->IsTexturingOn());
	}
};

// Light (It just support one point light, extending this should be easy)
struct cbLight : Light
{
	int viPadding[5];

	void Update( Light* pLight )
	{
		SetPosition(pLight->GetPosition());
	}
};

// Global illumination variables
struct cbGlobalIllumination
{
	float	fRandX,fRandY;				// 8 bytes - Rand Offset
	float	fFactorAnt;					// 4 bytes
	float	fFactorAct;					// 4 bytes
	unsigned int uiNumMuestras;				// 4 bytes
	int		vfPadding[3];				// 12 bytes

	cbGlobalIllumination()
	{
		uiNumMuestras = 0;
		fRandX = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
		fRandY = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
		fFactorAnt = 0.f;
		fFactorAct = 0.f;
	}

	void Update( unsigned int *pObj )
	{
		uiNumMuestras = *pObj;
		fRandX = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
		fRandY = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
		fFactorAnt = static_cast<float>(uiNumMuestras)/static_cast<float>(uiNumMuestras+1);
		fFactorAct = 1.0f/static_cast<float>(uiNumMuestras+1);
	}
};

struct cbScene
{
	unsigned int uiNumPrimitives;
	
	BBox box;
	unsigned int uiPadding;
	//Vector3 vfMin;
	//Vector3 vfMax;
	

	cbScene()
	{
		uiNumPrimitives = 0;
		box = BBox();
		//vfMin = Vector3(-1.f,-1.f,-1.f);
		//vfMax = Vector3(-1.f,-1.f,-1.f);
		uiPadding = 0;
	}

	void Update(Scene *  pScene)
	{
		uiNumPrimitives = pScene->GetModels()[0]->GetNumPrimitives();
		box = pScene->GetModels()[0]->GetAccelStructure()->WorldBound();
		//vfMin = pScene->GetModels()[0]->GetAccelStructure()->WorldBound().pMin;
		//vfMin = box.pMax;
		//vfMax = pScene->GetModels()[0]->GetAccelStructure()->WorldBound().pMax;
		//vfMin = pSLBVHData->vfMin;
		//vfMax = pSLBVHData->vfMax;
		//uiTreeLevel = pSLBVHData->uiTreeLevel;
	}
};