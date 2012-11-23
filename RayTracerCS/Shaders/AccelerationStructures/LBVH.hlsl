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

void LBVH_PreBuild( const unsigned int index, const unsigned int GIndex )
{
	if( index >= g_uiNumPrimitives )
	{
		g_uPrimitivesData[index].iId = -1;
		g_uPrimitivesId[index] = -1;
		g_uMortonCode[index].iId = -1;
		g_uMortonCode[index].iCode = 2147483647;
		return;
	}

	const float3 A = g_sVertices[g_sIndices[index*3]].vfPosition.xyz;
	const float3 B = g_sVertices[g_sIndices[index*3+1]].vfPosition.xyz;
	const float3 C = g_sVertices[g_sIndices[index*3+2]].vfPosition.xyz;
	
	const float3 BoxMin = min(A,min(B,C));
	const float3 BoxMax = max(A,max(B,C));
	const float3 Centroid =  A + B + C;

	g_uPrimitivesData[index].vfMin = BoxMin;
	g_uPrimitivesData[index].vfMax = BoxMax;
	g_uPrimitivesData[index].vfCentroid = Centroid;
	g_uPrimitivesData[index].iId = index;
	g_uPrimitivesData[index].iMaterialId = 0;
	g_uPrimitivesId[index] = index;

	//intfactor = 2097152.0f/intfactor; // Morton Code 64 bits

	/*for(unsigned int i = 1; i < BLOCK_SIZE_X; i <<= 1)
	{
		if(GIndex >= i)
		//if( (index & i) == i)
		{
			s_vfMin[GIndex] = min(s_vfMin[GIndex],s_vfMin[GIndex-i]);
			s_vfMax[GIndex] = max(s_vfMax[GIndex],s_vfMax[GIndex-i]);
		}
		GroupMemoryBarrierWithGroupSync();
	}

	GroupMemoryBarrierWithGroupSync();

	if(GIndex == 0)
	{
		s_vfMin[0] = s_vfMin[BLOCK_SIZE_X-1];
		s_vfMax[0] = s_vfMax[BLOCK_SIZE_X-1];

		g_uLVBH[index].vfMin = s_vfMin[GIndex];
		g_uLVBH[index].vfMax = s_vfMax[GIndex];

		g_uPrimitivesId[index].vfMin = s_vfMin[0];
		g_uPrimitivesId[index].vfMax = s_vfMax[0];
	}

	DeviceMemoryBarrierWithGroupSync();

	if(index == 0)
	{
		for(unsigned int i = 0; i < BLOCK_SIZE_X; i++)
		{
			g_uLVBH[0].vfMin = min(g_uLVBH[0].vfMin,g_uLVBH[i*BLOCK_SIZE_X].vfMin);
			g_uLVBH[0].vfMax = max(g_uLVBH[0].vfMax,g_uLVBH[i*BLOCK_SIZE_X].vfMax);
		}
	}*/

	//float3 intfactor = (s_MaxBox - s_MinBox);
	//intfactor = 2097152.0f/intfactor;

	//const float3 ABBBMax = g_vfMax;
	//const float3 ABBBMin = g_vfMin;
	//const float3 ABBBMax = float3(0.65191263,0.58088726,1.3596905);
	//const float3 ABBBMin = float3(-0.65191263,-0.58088726,-1.3596905);
	//const float3 ABBBMax = float3(0.65191263,0.58088726,1.3596905);
	//const float3 ABBBMin = float3(-0.65191263,-0.58088726,-1.3596905);
	//const float3 ABBBMax = 3*float3(0.283573,0.279901,0.285205);
	//const float3 ABBBMin = 3*float3(-0.283573,-0.279901,-0.285205);
	
	
	float3 intfactor = ((3*g_vfMax) - (3*g_vfMin));
	intfactor = 1024.f/intfactor;
	
	int3 tmp = int3((Centroid - (3*g_vfMin)) * intfactor);
	//g_uMortonCode[index].iCode = (tmp.z & 1) | ((tmp.y & 1) << 1) | ((tmp.x & 1) << 2);
	//double code = asdouble(0,(tmp.z & 1) | ((tmp.y & 1) << 1) | ((tmp.x & 1) << 2));
	uint code = (tmp.z & 1) | ((tmp.y & 1) << 1) | ((tmp.x & 1) << 2);
	
	int shift3 = 2, shift = 2;
	//[unroll(20)]for (int j = 1; j < 21; j++)
	for (int j = 1; j < 10; j++)
	{
		code |= (tmp.z & shift) << (shift3++);
		code |= (tmp.y & shift) << (shift3++);
		code |= (tmp.x & shift) << (shift3);
		//g_uMortonCode[index].iCode |= (tmp.z & shift) << (shift3++);
		//g_uMortonCode[index].iCode |= (tmp.y & shift) << (shift3++);
		//g_uMortonCode[index].iCode |= (tmp.x & shift) << (shift3);
		shift <<= 1;
	}

	g_uMortonCode[index].iId = index;
	g_uMortonCode[index].iCode = code;
	//asuint(code,g_uMortonCode[index].iCode[1],g_uMortonCode[index].iCode[0]);

	/*g_uPrimitivesId[index].vfMin = 1.f;
	g_uPrimitivesId[index].vfMax.x = g_vfMax.x;
	g_uPrimitivesId[index].vfMin.x = g_vfMin.x;
	g_uPrimitivesId[index].vfMin.y = g_vfMin.y;
	g_uPrimitivesId[index].vfMin.z = g_vfMin.z;*/

	if(index == 0)
	{
		g_uNodes[0].iPrimCount = -1;
		g_uNodes[0].iPrimPos = -1;
		g_uNodes[1].iPrimCount = (g_uiNumPrimitives << 3);
		g_uNodes[1].iPrimPos = 0;
		g_uNodes[1].vfMin = g_vfMin;
		g_uNodes[1].vfMax = g_vfMax;
	}	
}

void LBVH_Build( const unsigned int threadId, const unsigned int GIndex )
{
	const unsigned int level = g_uiTreeLevel; /* from 1 to d */
	const unsigned int offset = (1 << level);
	const unsigned int index = offset + (2*threadId);

	/* each thread creates a node, each level has 2^d nodes */
	if( threadId >= (offset>>1) )
	{
		return;
	}

	/* check if parent is a valid node */
	const uint parent = index >> 1;
	if( (g_uNodes[parent].iPrimCount) < 0 )
	{
		/* not a valid node */
		g_uNodes[index].iPrimPos = -2147483647;
		g_uNodes[index].iPrimCount = (-2147483647);
		g_uNodes[index+1].iPrimPos = -2147483647;
		g_uNodes[index+1].iPrimCount = (-2147483647);
		return;
	}

	//int isleaf = ) ? 4 : 0;
	int isleaf = 0;
	if( index >= (1<<(g_uiSLBVHDepth-1)) )
	{
		isleaf = 4;
	}
	
	/* check if node is leaf */
	int a = g_uNodes[parent].iPrimPos;
	int b = g_uNodes[parent].iPrimPos + (g_uNodes[parent].iPrimCount >> 3) - 1;
	int mask = g_uMortonCode[a].iCode ^ g_uMortonCode[b].iCode;
	if( mask == 0 )
	{
		/* it is a leaf, make invalid both children */
		g_uNodes[index].iPrimPos = -2147483647;
		g_uNodes[index].iPrimCount = (-2147483647);
		g_uNodes[index+1].iPrimPos = -2147483647;
		g_uNodes[index+1].iPrimCount = (-2147483647);

		g_uNodes[parent].iPrimCount |= 4;
		//g_uNodes[parent].iPrimCount = (g_uNodes[parent].iPrimCount << 3) | (4) | (axis);
		return;
	}

	/* check if node is internal and find the cut */
	int p = firstbithigh(mask);
	mask = 1 << p;
	[allow_uav_condition]
	while( (b-a) > 1 )
	{
		/* binary search */
		const int c = ((b+a) >> 1);
		if( (g_uMortonCode[c].iCode & mask) == 0)
		{
			a = c;
		}
		else
		{
			b = c;
		}
		/* a and b store the position of the bit change 0->1 */
	}

	/* leaf */
	//const int axis = (((2-(p%3))+g_uiTreeLevel)%3);
	//const int axis = 2-(p%3);

	int axis = 0;
	float xDiff = abs(g_uNodes[parent].vfMax.x - g_uNodes[parent].vfMin.x);
	float yDiff = abs(g_uNodes[parent].vfMax.y - g_uNodes[parent].vfMin.y);
	float zDiff = abs(g_uNodes[parent].vfMax.z - g_uNodes[parent].vfMin.z);

	if(yDiff < xDiff)
		axis = 1;
	if(zDiff < yDiff)
		axis = 2;

	/*if((abs(b - g_uNodes[index].iPrimPos)/(g_uNodes[parent].iPrimCount>>3)) > 0.8)
	{
		b = (g_uNodes[parent].iPrimCount>>3)/2;
	}*/

	g_uNodes[index].iPrimPos = g_uNodes[parent].iPrimPos;
	g_uNodes[index].iPrimCount = ((b - g_uNodes[index].iPrimPos)<< 3)  | isleaf | axis;
	g_uNodes[index+1].iPrimPos = b;
	g_uNodes[index+1].iPrimCount = (((g_uNodes[parent].iPrimCount>>3)-(g_uNodes[index].iPrimCount>>3)) << 3) | isleaf | axis;
	//g_uNodes[index+1].iPrimCount = g_uNodes[parent].iPrimCount-g_uNodes[index].iPrimCount;
	//g_uNodes[parent].iPrimCount = (g_uNodes[parent].iPrimCount & 7); // PrimCount = 0;
}

void LBVH_BuildAABB(const int threadId, const int GIndex)
{
	const int level = g_uiTreeLevel; /* from 1 to d */
	const int offset = (1 << level);
	const int index = offset + threadId;
	
	/* if it is invalid, skip */
	if( g_uNodes[index].iPrimPos < 0)
	{
		return;
	}

	/* if it is a leaf, take the AABB of the primitives */
	if( g_uNodes[index].iPrimCount & 4 )
	{
		const int MortonOffset = g_uNodes[index].iPrimPos;
		g_uNodes[index].vfMax = g_uPrimitivesData[g_uMortonCode[MortonOffset].iId].vfMax;
		g_uNodes[index].vfMin = g_uPrimitivesData[g_uMortonCode[MortonOffset].iId].vfMin;
		const int primCount = (g_uNodes[index].iPrimCount >> 3);
		[allow_uav_condition]
		for( int j = 1; j < primCount; j++)
		{
			/* get max and min AABB */
			const float3 NodeAABBMax = g_uNodes[index].vfMax;
			const float3 NodeAABBMin = g_uNodes[index].vfMin;
			const float3 PrimitiveAABBMax = g_uPrimitivesData[g_uMortonCode[MortonOffset+j].iId].vfMax;
			const float3 PrimitiveAABBMin = g_uPrimitivesData[g_uMortonCode[MortonOffset+j].iId].vfMin;
			g_uNodes[index].vfMax = max(NodeAABBMax, PrimitiveAABBMax);
			g_uNodes[index].vfMin = min(NodeAABBMin, PrimitiveAABBMin);
		}
	}
	/* if it is an internal node, take the union of the children's AABB */
	else
	{
		const float3 NodeLeftAABBMax = g_uNodes[index << 1].vfMax;
		const float3 NodeLeftAABBMin = g_uNodes[index << 1].vfMin;
		const float3 NodeRightAABBMax = g_uNodes[(index << 1) + 1].vfMax;
		const float3 NodeRightAABBMin = g_uNodes[(index << 1) + 1].vfMin;

		g_uNodes[index].vfMax = max(NodeLeftAABBMax, NodeRightAABBMax);
		g_uNodes[index].vfMin = min(NodeLeftAABBMin, NodeRightAABBMin);
	}
}

// ------------------------------------------
// LBVH intersection function
// ------------------------------------------
Intersection LBVH_IntersectP2(Ray ray, unsigned int nodeNum)
{	
	Intersection bIntersection;
	bIntersection.iTriangleId = -1;
	bIntersection.fU = -1;
	bIntersection.fV = -1;
	bIntersection.fT = ray.fMaxT;
	//bIntersection.fT = ray.fMaxT;
	//bIntersection.iVisitedNodes = 0;
	bIntersection.iRoot = 1;

	int iTriangleId = -1;
	float2 T;
	const float3 invDir = float3(1.f/ray.vfDirection);
	const int3 dirIsNeg = (invDir < 0.f) ? 1 : 0;

	int result = 0;
	int trail = (1 << (firstbithigh(nodeNum)));

	[allow_uav_condition]
	while(true)
	{
		int p = 0;

		[allow_uav_condition]
		while( !(g_uNodes[nodeNum].iPrimCount & 4))
		{
			T = IntersectBox(ray.vfOrigin, invDir, nodeNum);
			p = firstbitlow(trail + 1); 
			result++;
			if ((T[0] > bIntersection.fT) || (T[1] < 0.0f))
			{
				
				//does not intersect, change to next node
				trail = (trail >> p) + 1;
				nodeNum = (nodeNum >> p) ^ 1;
			}
			else
			{
				trail = trail << 1;	
				nodeNum = (nodeNum << 1) | (dirIsNeg[((g_uNodes[nodeNum].iPrimCount)) & 3] );
			}
			if(trail <= 1) break;
		}

		if(trail <= 1) break;

		const unsigned int iPrimCount = g_uNodes[nodeNum].iPrimCount >> 3;
		const unsigned int iPrimPos = g_uNodes[nodeNum].iPrimPos;
		p = firstbitlow(trail + 1); 
	
		//it is a leaf, compare against all the primitives and asign next node
		[allow_uav_condition]for(unsigned int i = 0; i < iPrimCount; ++i)
		{
			result += 65536;
			iTriangleId = g_uPrimitivesId[iPrimPos+i];

			int offset = iTriangleId*3;
			float3 A = float3(g_sVertices[g_sIndices[offset]].vfPosition.xyz);
			float3 B = float3(g_sVertices[g_sIndices[offset+1]].vfPosition.xyz);
			float3 C = float3(g_sVertices[g_sIndices[offset+2]].vfPosition.xyz);
			
			Intersection cIntersection = getIntersection(ray,A,B,C);
			if( (ray.iTriangleId != iTriangleId)
				&& RayTriangleTest(cIntersection) 
				&& (cIntersection.fT < bIntersection.fT)
				&& (cIntersection.fT >=0.f))
			{
				bIntersection = cIntersection;
				bIntersection.iTriangleId = iTriangleId;
				bIntersection.iRoot = nodeNum;
					
			}
		}
		trail = (trail >> p) + 1;
		nodeNum = (nodeNum >> p) ^ 1;

		if(trail <= 1) break;
	} 
	
	bIntersection.iVisitedNodes = result;

	return bIntersection;
}

// ------------------------------------------
// LBVH intersection function
// ------------------------------------------
Intersection LBVH_IntersectP(Ray ray, unsigned int nodeNum)
{	
	Intersection bIntersection;
	bIntersection.iTriangleId = -1;
	bIntersection.fU = -1;
	bIntersection.fV = -1;
	bIntersection.fT = ray.fMaxT;
	//bIntersection.fT = ray.fMaxT;
	//bIntersection.iVisitedNodes = 0;
	bIntersection.iRoot = 1;

	int iTriangleId = -1;
	float2 T;
	const float3 invDir = float3(1.f/ray.vfDirection);
	const int3 dirIsNeg = (invDir < 0.f) ? 1 : 0;

	int result = 0;
	//nodeNum = 1;
	//int trail = (3 << (firstbithigh(nodeNum)))-1;
	int trail = (1 << (firstbithigh(nodeNum)));

	[allow_uav_condition]do
	{
		// Check if this node intersects with the ray
		T = IntersectBox(ray.vfOrigin, invDir, nodeNum);
		const int p = firstbitlow(trail + 1); 
		result++;
		if ((T[0] > bIntersection.fT) || (T[1] < 0.0f))
		{
			//does not intersect, change to next node
			trail = (trail >> p) + 1;
			nodeNum = (nodeNum >> p) ^ 1;
		}
		else if(g_uNodes[nodeNum].iPrimCount & 4)
		{
			const unsigned int iPrimCount = g_uNodes[nodeNum].iPrimCount >> 3;
			const unsigned int iPrimPos = g_uNodes[nodeNum].iPrimPos;

			//it is a leaf, compare against all the primitives and asign next node
			[allow_uav_condition]for(unsigned int i = 0; i < iPrimCount; ++i)
			{
				result += 65536;
				iTriangleId = g_uPrimitivesId[iPrimPos+i];

				int offset = iTriangleId*3;
				float3 A = float3(g_sVertices[g_sIndices[offset]].vfPosition.xyz);
				float3 B = float3(g_sVertices[g_sIndices[offset+1]].vfPosition.xyz);
				float3 C = float3(g_sVertices[g_sIndices[offset+2]].vfPosition.xyz);

				//cThereonst int axis = ((g_uNodes[nodeNum].iPrimCount)>>1) & 3;
				
				Intersection cIntersection = getIntersection(ray,A,B,C);
				if( (ray.iTriangleId != iTriangleId)
					&& RayTriangleTest(cIntersection) 
					&& (cIntersection.fT < bIntersection.fT)
					&& (cIntersection.fT >=0.f))
				{
					bIntersection = cIntersection;
					bIntersection.iTriangleId = iTriangleId;
					bIntersection.iRoot = nodeNum;
					ray.fMaxT = bIntersection.fT;
				}
			}
			trail = (trail >> p) + 1;
			nodeNum = (nodeNum >> p) ^ 1;
		}
		else
		{
			trail = trail << 1;	
			nodeNum = (nodeNum << 1) | (dirIsNeg[((g_uNodes[nodeNum].iPrimCount)) & 3] );
		}
	} 
	while (trail > 1);
	
	bIntersection.iVisitedNodes = result;

	return bIntersection;
}