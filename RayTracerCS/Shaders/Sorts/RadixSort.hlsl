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

groupshared int sData[BLOCK_SIZE_X];
groupshared int sTotalFalses;

[numthreads(BLOCK_SIZE_X, 1, 1)]
void CSRadixSort(uint3 DTid : SV_DispatchThreadID, 
				 uint groupIdx : SV_GroupID,
				 uint GIndex : SV_GroupThreadID )
{
	const int threadNumber = g_uBuffer[DTid.x].iValue;
	DeviceMemoryBarrierWithGroupSync();
	if(GIndex == BLOCK_SIZE_X-1) 
	{
		sTotalFalses = 0;
	}
	
	// Set current bit value 0-1
	int b = (threadNumber >> g_iCurrentBit) & 0x01;
	sData[GIndex] = !b;
	GroupMemoryBarrierWithGroupSync();

	// Compute totalFalses
	if(GIndex == BLOCK_SIZE_X-1)
	{
		sTotalFalses += sData[GIndex];
	}

	// Scan values of 'b' for each element
	for(unsigned int s = 1; s < BLOCK_SIZE_X; s <<= 1)
	{
		if(GIndex >= s)
		{
			InterlockedAdd(sData[GIndex],sData[GIndex-s]);
		}
		GroupMemoryBarrierWithGroupSync();
	}

	/*GroupMemoryBarrierWithGroupSync();
	if(GIndex > 32) sData[GIndex] += sData[GIndex-32];
	GroupMemoryBarrierWithGroupSync();
	if(GIndex > 64) sData[GIndex] += sData[GIndex-64];
	GroupMemoryBarrierWithGroupSync();
	if(GIndex > 128) sData[GIndex] += sData[GIndex-128];
	GroupMemoryBarrierWithGroupSync();
	if(GIndex > 256) sData[GIndex] += sData[GIndex-256];
	GroupMemoryBarrierWithGroupSync();
	if(GIndex > 512) sData[GIndex] += sData[GIndex-512];
	GroupMemoryBarrierWithGroupSync();*/
	
	sData[GIndex] -= !b;
	const int f = sData[GIndex];

	// Compute totalFalses
	//GroupMemoryBarrierWithGroupSync();
	AllMemoryBarrierWithGroupSync();
	if(GIndex == BLOCK_SIZE_X-1) 
	{
		sTotalFalses += sData[GIndex];
	}
	//GroupMemoryBarrierWithGroupSync();
	AllMemoryBarrierWithGroupSync();

	// Calculate the index 'd' for the current element
	const int t = GIndex - f + sTotalFalses;
	int d = b ? t : f;
	d += groupIdx*BLOCK_SIZE_X;

	/* debug data */
	g_uBuffer[DTid.x].b = b;
	g_uBuffer[DTid.x].e = !b;
	g_uBuffer[DTid.x].f = f;
	g_uBuffer[DTid.x].t = t;
	g_uBuffer[DTid.x].iTotalFalses = sTotalFalses;
	g_uBuffer[DTid.x].d = d;
	
	/* Swap elements and copy to final position */
	DeviceMemoryBarrierWithGroupSync();
	g_uBuffer[d].iValue = threadNumber;
	DeviceMemoryBarrierWithGroupSync();
	g_uBuffer[DTid.x].iPastValue = threadNumber;
}