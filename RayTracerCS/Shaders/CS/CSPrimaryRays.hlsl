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

//-------------------------------------------------------------------------------------------
// PRIMARY RAYS STAGE
//-------------------------------------------------------------------------------------------
// Primary rays.
#define focallength (0.008)
#define fnumber (0.3f)

[numthreads(BLOCK_SIZE_X, BLOCK_SIZE_Y, 1)]
void CSGeneratePrimaryRays(uint3 DTid : SV_DispatchThreadID, uint GIndex : SV_GroupIndex )
{
#ifdef GLOBAL_ILLUM
	float2 indexasfloat = float2(DTid.x,DTid.y)/float(N);
	float2 jitterOffset = indexasfloat+float2(g_fRandX,g_fRandY);
	float2 tx_Random = g_sRandomTx.SampleLevel(g_ssSampler, jitterOffset, 0).zw;
	tx_Random.y=2.0*tx_Random.y-1.0;
	float lensradi = 0.0*focallength/fnumber;
	float2 lensrandom = lensradi*g_sRandomTx.SampleLevel(g_ssSampler, jitterOffset, 0).xy;
	float2 jitter = 2.0f*tx_Random-1.0f;
	float inverse = 1.0f/(float(N));
	float y = -float(2.f * DTid.y + jitter.x + 1.f - N) * inverse;
	float x = float(2.f * DTid.x + jitter.y + 1.f - N) * inverse;	
	float z = 2.0f;	
	float4 aux = (mul(float4(lensrandom.x,lensrandom.y,0,1.f),g_mfWorld));
#else
	float inverse = 1.0f/(float(N));
	float y = -float(2.f * DTid.y + 1.f - N) * inverse;
	float x = float(2.f * DTid.x + 1.f - N) * inverse;	
	float z = 2.0f;
	float4 aux = (mul(float4(0,0,0,1.f),g_mfWorld));
#endif

	// Create new ray from the camera position to the pixel position
	Ray ray;
	float scale=focallength/abs(0.035f*z);
	ray.vfOrigin = aux.xyz/aux.w;
	aux = mul(float4(scale*x,scale*y,scale*z,1.f),g_mfWorld);
	float3 vfPixelPosition = aux.xyz/aux.w;
	ray.vfDirection = normalize(vfPixelPosition-ray.vfOrigin);
	ray.fMaxT = 10000000000000000.f;
	ray.fMinT = 0;
	ray.vfReflectiveFactor = float3(1.f,1.f,1.f);
	ray.iTriangleId = -1;

	unsigned int index = DTid.y * N + DTid.x;
	// Copy ray to global UAV
	g_uRays[index] = ray;
	// Initialize accumulation buffer and result buffer
#ifdef GLOBAL_ILLUM
	g_uAccumulation[index] *= g_fFactorAnt;
#else
	g_uAccumulation[index] = 0.0f;
#endif

	g_uResultTexture[DTid.xy] = 0.f;
}