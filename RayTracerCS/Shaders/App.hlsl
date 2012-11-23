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

// Ray Tracing on the Compute Shader.

// Use or not global illumination
//#define GLOBAL_ILLUM

// Include constant buffers and user-defined structures
#include "ConstantBuffers.hlsl"
#include "Structures.hlsl"

/* define type of data to sort on BitonicSort */
#define MORTON_CODE
//#define UINT

// SRVs
//StructuredBuffer<int>				g_sPrimitivesId		:	register( t0 );
//StructuredBuffer<Node>			g_sNodes			:	register( t1 );
StructuredBuffer<Vertex>			g_sVertices			:	register( t0 );
StructuredBuffer<DWORD>				g_sIndices			:	register( t1 );
Texture2DArray						g_sTextures			:	register( t2 );
Texture2DArray						g_sSpecularMaps		:	register( t3 );
Texture2DArray						g_sNormalMaps		:	register( t4 );
StructuredBuffer<PrimitiveData>		g_sPrimitivesData	:	register( t5 );
Texture2D							g_sRandomTx			:	register( t6 );
TextureCube							g_sEnvironmentTx	:	register( t7 );
StructuredBuffer<Material>			g_sMaterials		:	register( t8 );
#ifdef MORTON_CODE
StructuredBuffer<MortonCode>		g_sInput			:	register( t9 );
#elif defined (UINT)
StructuredBuffer<int>				g_sInput			:	register( t9 );
#endif	

// UAVs
RWTexture2D<float4>					g_uResultTexture	:	register( u0 );
RWStructuredBuffer<Ray>				g_uRays				:	register( u1 );
RWStructuredBuffer<Intersection>	g_uIntersections	:	register( u2 );
RWStructuredBuffer<float4>			g_uAccumulation		:	register( u3 );
RWStructuredBuffer<int>				g_uPrimitivesId		:	register( u4 );
RWStructuredBuffer<Node>			g_uNodes			:	register( u5 );
#ifdef MORTON_CODE
RWStructuredBuffer<MortonCode>		g_uMortonCode		:	register( u6 );
#elif defined (UINT)
RWStructuredBuffer<int>				g_uMortonCode		:	register( u6 );
#endif	
RWStructuredBuffer<PrimitiveData>	g_uPrimitivesData	:	register( u7 );

// Samplers
SamplerState g_ssSampler : register(s0);

// Include application files
//#include "Sorts/RadixSort.hlsl"

#include "Sorts/BitonicSort.hlsl"
#include "Core/Intersection.hlsl"
#include "Core/MortonCode.hlsl"
#include "AccelerationStructures/LBVH.hlsl"
#include "AccelerationStructures/BVH.hlsl"
#include "AccelerationStructures/Simple.hlsl"
#include "CS/CSPrimaryRays.hlsl"
#include "CS/CSIntersections.hlsl"
#include "CS/CSBuildStructure.hlsl"
#ifndef GLOBAL_ILLUM
#include "CS/CSComputeColorRT.hlsl"
#else // global illumination
#include "CS/CSComputeColorGI.hlsl"
#endif