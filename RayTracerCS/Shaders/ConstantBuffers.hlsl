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

// ------------------------------------------
// Constant Buffers	(Multiple of 32 bytes)
// ------------------------------------------
cbuffer cbCamera : register( b0 )	// 64 bytes
{
	float4x4 g_mfWorld;		// 64 bytes
};

cbuffer cbInputOutput : register( b1 )	//64  bytes
{
	int	g_bIsShadowOn;				// 4 byte
	int	g_bIsPhongShadingOn;		// 4 byte
	int	g_bIsNormalMapspingOn;		// 4 byte
	int	g_bIsGlossMappingOn;		// 4 bytes
	int	g_iAccelerationStructure;	// 4 byte
	int	g_iEnvMappingFlag;			// 4 bytes
	uint g_iNumBounces;				// 4 bytes
	int bIsRebuildStructureOn;		// 4 bytes
	int g_bIsTexturingOn;			// 4 bytes
	int g_IOPadding[7];				// 28 bytes
};

cbuffer cbLight : register( b2 )	// 32 bytes
{
	float3 g_vfLightPosition;		// 12 bytes
	int g_vfPaddingLight[5];		// 20 bytes
};

cbuffer cbGlobalIllumination : register( b3 ) //	32 bytes
{
	float g_fRandX,g_fRandY;	// 8 bytes
	float g_fFactorAnt;			// 4 bytes
	float g_fFactorAct;			// 4 bytes
	uint g_uiNumMuestras;		// 4 bytes
	int3 g_vfPadding;			// 12 bytes
};

cbuffer cbScene : register( b4 )	// 32 bytes
{
	uint g_uiNumPrimitives;
	float3 g_vfMin;
	float3 g_vfMax;
	uint g_viTrash;
};

cbuffer cbBitonicSort : register( b5 )	// 32 bytes
{
	//Multiple of 32 bytes
	unsigned int g_iLevel;
    unsigned int g_iLevelMask;
    unsigned int g_iWidth;
    unsigned int g_iHeight;
	uint g_viPadding11[4];
};

cbuffer cbRadixSort : register( b6 )	// 32 bytes
{
	//Multiple of 32 bytes
	int g_iCurrentBit;			// 4 bytes
	int g_iNumElements;			// 4 bytes
	float3 g_vfPadding1;		// 12 bytes
	float3 g_vfPadding2;		// 12 bytes
};

cbuffer cbSLBVH : register( b7 )	// 32 bytes
{
	//Multiple of 32 bytes
	unsigned int g_uiTreeLevel;			// 4 bytes
	unsigned int g_uiSLBVHDepth;
	int g_vfPaddingB7[6];			// 4 bytes
};

cbuffer cbBBVH : register( b8 )
{
	//Multiple of 32 bytes
	unsigned int g_uiCurrentNode;	// 4 bytes
	unsigned int g_uiBlockSize_x;	// 4 bytes
	unsigned int g_uiBlockSize_y;	// 4 bytes
	unsigned int g_uiBlockSize_z;	// 4 bytes
	int g_vfPaddingBBVH[4];
};