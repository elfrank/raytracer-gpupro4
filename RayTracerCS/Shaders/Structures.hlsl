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
// Structures
// ------------------------------------------
struct Node						// 32 bytes	
{
	// Bounds
	float3 vfMin;				// 12 bytes
	float3 vfMax;				// 12 bytes

	int iPrimCount;				// 4 bytes
	int iPrimPos;				// 4 bytes
};

struct Ray						// 48 bytes
{
	float3 vfOrigin;			// 12 bytes
	float3 vfDirection;			// 12 bytes
	float3 vfReflectiveFactor;	// 12 bytes
	float fMinT,fMaxT;			// 8 bytes
	int iTriangleId;			// 4 bytes
};

struct Vertex					// 32 bytes
{
	float3 vfPosition;			// 12 bytes
	float3 vfNormal;			// 12 bytes
	float2 vfUvs;				// 8 bytes
};

struct Intersection				// 24 bytes
{
	int iTriangleId;			// 4 bytes
	float fU, fV, fT;			// 12 bytes
	int iVisitedNodes;			// 4 bytes
	int iRoot;					// 4 bytes
};

struct MortonCode				// 12 bytes
{
	//uint2 iCode;				// 8 bytes
	uint iCode;					// 8 bytes
	int iId;					// 4 bytes
};

// PrimitivesData
struct PrimitiveData			// 44 bytes
{
	float3 vfCentroid;			// 12 bytes
	float3 vfMin;				// 12 bytes
	float3 vfMax;				// 12 bytes
	int iMaterialId;			// 4 bytes
	int iId;					// 4 bytes
};

struct Material // 64 bytes
{
	float4			fDiffuse; // 16 bytes
	float4			fAmbient; // 16 bytes
	float4			fSpecular; // 16 bytes
	float			fShininess; // 4 bytes
	float			fAlpha; // 4 bytes
	unsigned int	uiMaterialId; // 4 bytes, id of the material
	unsigned int	uiPadding; // 4 bytes
};

struct RadixDebug
{
	int iValue;
	int iPastValue;
	int f;
	int b;
	int e; // !b
	int d; // final position
	int t;
	int iTotalFalses;
};