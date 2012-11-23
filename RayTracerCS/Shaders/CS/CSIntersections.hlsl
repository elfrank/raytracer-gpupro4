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

//--------------------------------------------------------------------------------------------------------------------
// INTERSECTION STAGE
//--------------------------------------------------------------------------------------------------------------------
[numthreads(BLOCK_SIZE_X, BLOCK_SIZE_Y, 1)]
void CSComputeIntersections(uint3 DTid : SV_DispatchThreadID)
{
	unsigned int index = DTid.y * N + DTid.x;

    if ( g_uRays[index].iTriangleId > (-2) )
	{
		// Uncomment a line to select an acceleration structure
		if(g_iAccelerationStructure == 0)
		{
			g_uIntersections[index] = BVH_IntersectP(g_uRays[index]);
		}
		else if(g_iAccelerationStructure == 9)
		{
    		g_uIntersections[index] = LBVH_IntersectP(g_uRays[index], g_uIntersections[index].iRoot);
		}
	}
	else
	{
		g_uIntersections[index].iTriangleId = -2;
	}
}