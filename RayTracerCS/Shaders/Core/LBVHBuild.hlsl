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

groupshared float3 s_vfMin[BLOCK_SIZE_X];
groupshared float3 s_vfMax[BLOCK_SIZE_X];

void LBVHBuild( const unsigned int index )
{
	const float3 A = g_sVertices[g_sIndices[DTid.x*3]].vfPosition.xyz;
	const float3 B = g_sVertices[g_sIndices[DTid.x*3+1]].vfPosition.xyz;
	const float3 C = g_sVertices[g_sIndices[DTid.x*3+2]].vfPosition.xyz;

	g_uPrimitivesData[DTid.x].vfMin = min(A,min(B,C));
	g_uPrimitivesData[DTid.x].vfMax = max(A,max(B,C));
	g_uPrimitivesData[DTid.x].vfCentroid =  A + B + C;

	s_vfMin[GIndex] = g_uPrimitivesData[DTid.x].vfMin;
	s_vfMax[GIndex] = g_uPrimitivesData[DTid.x].vfMax;

	GroupMemoryBarrierWithGroupSync();

	for(unsigned int i = 1; i < BLOCK_SIZE_X; i <<= 1)
	{
		if( (DTid.x % (2*i)) == 0)
		{
			s_vfMin[GIndex] = min(s_vfMin[GIndex],s_vfMin[GIndex+i]);
			s_vfMax[GIndex] = max(s_vfMax[GIndex],s_vfMax[GIndex+i]);
		}
		GroupMemoryBarrierWithGroupSync();
	}

	GroupMemoryBarrierWithGroupSync();

	if(GIndex == 0)
	{
		g_uLVBH[DTid.x].vfMin = s_vfMin[0];
		g_uLVBH[DTid.x].vfMax = s_vfMax[0];
	}

	DeviceMemoryBarrierWithGroupSync();

	if(DTid.x == 0)
	{
		for(unsigned int i = BLOCK_SIZE_X; i < BLOCK_SIZE_X*BLOCK_SIZE_X; i += BLOCK_SIZE_X)
		{
			g_uLVBH[0].vfMin = min(g_uLVBH[0].vfMin,g_uLVBH[i].vfMin);
			g_uLVBH[0].vfMax = max(g_uLVBH[0].vfMax,g_uLVBH[i].vfMax);
		}
	}

	DeviceMemoryBarrierWithGroupSync();
	
	float3 intfactor = (g_uLVBH[0].vfMax - g_uLVBH[0].vfMin);
	intfactor = 2097152.0f/intfactor;

	int3 tmp = int3((g_uPrimitivesData[DTid.x].vfCentroid - g_uPrimitivesData[DTid.x].vfMin) * intfactor);
	g_uMortonCode[DTid.x].iCode = (tmp.z & 1) | ((tmp.y & 1) << 1) | ((tmp.x & 1) << 2);
	g_uMortonCode[DTid.x].iId = DTid.x;
	int shift3 = 2, shift = 2;+
	for (int j = 1; j < 21; j++)
	{
		g_uMortonCode[DTid.x].iCode |= (tmp.z & shift) << (shift3++);
		g_uMortonCode[DTid.x].iCode |= (tmp.y & shift) << (shift3++);
		g_uMortonCode[DTid.x].iCode |= (tmp.x & shift) << (shift3);
		shift <<= 1;
	}

	// START SORT

	// Scan

	// END SORT

	//g_uPrimitivesData[TriangleID] = g_uMortonCode[TriangleID].iId;

}


/*
groupshared float3 s_vfMin[BLOCK_SIZE_X];
groupshared float3 s_vfMax[BLOCK_SIZE_X];
//[numthreads(BLOCK_SIZE_X, 1, 1)]
[numthreads(BLOCK_SIZE_X, 1, 1)]
void CSInitBuild( uint3 DTid : SV_DispatchThreadID, uint GIndex : SV_GroupIndex )
{
	const unsigned int index = DTid.x;

	//g_uMortonCode[index].iCode = g_iNumElements-index-1;
	//g_uMortonCode[index].iCode = BITONIC_BLOCK_SIZE*BITONIC_BLOCK_SIZE-index-1;
	//g_uMortonCode[index].iId = g_uMortonCode[index].iCode;
	//g_uMortonCode[index].ant = g_uMortonCode[index].iCode;

	const float3 A = g_sVertices[g_sIndices[DTid.x*3]].vfPosition.xyz;
	const float3 B = g_sVertices[g_sIndices[DTid.x*3+1]].vfPosition.xyz;
	const float3 C = g_sVertices[g_sIndices[DTid.x*3+2]].vfPosition.xyz;

	g_uPrimitivesData[DTid.x].vfMin = min(A,min(B,C));
	g_uPrimitivesData[DTid.x].vfMax = max(A,max(B,C));
	g_uPrimitivesData[DTid.x].vfCentroid =  A + B + C;

	s_vfMin[GIndex] = g_uPrimitivesData[DTid.x].vfMin;
	s_vfMax[GIndex] = g_uPrimitivesData[DTid.x].vfMax;

	GroupMemoryBarrierWithGroupSync();

	for(unsigned int i = 1; i < BLOCK_SIZE_X; i <<= 1)
	{
		if( (DTid.x % (2*i)) == 0)
		{
			s_vfMin[GIndex] = min(s_vfMin[GIndex],s_vfMin[GIndex+i]);
			s_vfMax[GIndex] = max(s_vfMax[GIndex],s_vfMax[GIndex+i]);
		}
		GroupMemoryBarrierWithGroupSync();
	}

	GroupMemoryBarrierWithGroupSync();

	if(GIndex == 0)
	{
		g_uLVBH[DTid.x].vfMin = s_vfMin[0];
		g_uLVBH[DTid.x].vfMax = s_vfMax[0];
	}

	DeviceMemoryBarrierWithGroupSync();

	if(DTid.x == 0)
	{
		for(unsigned int i = BLOCK_SIZE_X; i < BLOCK_SIZE_X*BLOCK_SIZE_X; i += BLOCK_SIZE_X)
		{
			g_uLVBH[0].vfMin = min(g_uLVBH[0].vfMin,g_uLVBH[i].vfMin);
			g_uLVBH[0].vfMax = max(g_uLVBH[0].vfMax,g_uLVBH[i].vfMax);
		}
	}

	DeviceMemoryBarrierWithGroupSync();
	
	float3 intfactor = (g_uLVBH[0].vfMax - g_uLVBH[0].vfMin);
	intfactor = 2097152.0f/intfactor;

	int3 tmp = int3((g_uPrimitivesData[DTid.x].vfCentroid - g_uPrimitivesData[DTid.x].vfMin) * intfactor);
	g_uMortonCode[DTid.x].iCode = (tmp.z & 1) | ((tmp.y & 1) << 1) | ((tmp.x & 1) << 2);
	g_uMortonCode[DTid.x].iId = DTid.x;
	int shift3 = 2, shift = 2;
	for (int j = 1; j < 21; j++)
	{
		g_uMortonCode[DTid.x].iCode |= (tmp.z & shift) << (shift3++);
		g_uMortonCode[DTid.x].iCode |= (tmp.y & shift) << (shift3++);
		g_uMortonCode[DTid.x].iCode |= (tmp.x & shift) << (shift3);
		shift <<= 1;
	}

	// START SORT

	// Scan

	// END SORT

	//g_uPrimitivesData[TriangleID] = g_uMortonCode[TriangleID].iId;
}*/