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

[numthreads(BLOCK_SIZE_X, 1, 1)]
void CSPreBuildStructure(uint3 DTid : SV_DispatchThreadID, uint GIndex : SV_GroupIndex )
{
	//g_uMortonCode[DTid.x].iId = -1;
	//return;
	if(g_iAccelerationStructure == 9)
	{
		LBVH_PreBuild(DTid.x, GIndex);
	}
}

[numthreads(BLOCK_SIZE_X, 1, 1)]
void CSBuildStructure(uint3 DTid : SV_DispatchThreadID, uint GIndex : SV_GroupIndex )
{
	if(g_iAccelerationStructure == 9)
	{
		LBVH_Build(DTid.x, GIndex);
	}
}

[numthreads(BLOCK_SIZE_X, 1, 1)]
void CSPosBuildStructure(uint3 DTid : SV_DispatchThreadID, uint GIndex : SV_GroupIndex )
{
	if(g_iAccelerationStructure == 9)
	{
		LBVH_BuildAABB(DTid.x, GIndex);
	}
}

[numthreads(BLOCK_SIZE_X, 1, 1)]
//void CSReduce(uint3 DTid : SV_DispatchThreadID, uint GIndex : SV_GroupIndex )
void CSReduce(uint gid : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID, uint3 groupIdx : SV_GroupID )
{

}