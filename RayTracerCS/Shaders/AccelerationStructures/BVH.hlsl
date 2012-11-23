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
// BVH intersection function
// ------------------------------------------
Intersection BVH_IntersectP2(Ray ray)
{	
	// Initialize variables
	int iTrId = -1;
	float3 vfInvDir = float3(1.0f / ray.vfDirection);	
	
	int stack[35];
	int iStackOffset = 0;
	int iNodeNum = 0;
	int result = 0;
	
	// Initialize the current and best intersection.
	// They are empty at the beginning.
	Intersection cIntersection;
	Intersection bIntersection;
	bIntersection.iTriangleId = -1;
	bIntersection.fU = -1;
	bIntersection.fV = -1;
	bIntersection.fT = ray.fMaxT;
	bIntersection.iRoot = 0;
	bIntersection.iVisitedNodes = 0;
	
	[allow_uav_condition]
	while( true )
	{	
		// Perform ray-box intersection test
		//float2 T = BVH_IntersectBox(ray.vfOrigin, vfInvDir, iNodeNum);
		result++;

		[allow_uav_condition]
		while( g_uNodes[iNodeNum].iPrimCount <= 0 )
		{
			float2 T = IntersectBox(ray.vfOrigin, vfInvDir, iNodeNum);
			if ((T[0] > bIntersection.fT) || (T[1] < 0.0f))
			{
				// If the stack is empty, the traversal ends
				if(iStackOffset == 0) break;	
				// Pop a new node from the stack
				iNodeNum = stack[--iStackOffset];
			}
			else
			{
				// Depending on the ray direction and the split-axis,
				// the order of the children changes on the stack.
				int dirIsNeg[3] = { vfInvDir < 0 };
				// -g_uNodes[iNodeNum].iPrimCount is the split axis: 0-1-2 (x-y-z)
				const int aux = dirIsNeg[-g_uNodes[iNodeNum].iPrimCount];
				// aux replaces an if/else statement which improves traversal a little bit
				stack[iStackOffset++] = (iNodeNum+1)*aux + (1-aux)*g_uNodes[iNodeNum].iPrimPos;
				iNodeNum = g_uNodes[iNodeNum].iPrimPos*aux + (1-aux)*(iNodeNum+1);
			}
		}
		
		if(iStackOffset == 0) break;	

		[allow_uav_condition]
		while( g_uNodes[iNodeNum].iPrimCount > 0 )
		{
			[allow_uav_condition]
			for( int i = g_uNodes[iNodeNum].iPrimCount; i >= 0 ; --i )
			{
				result += 65536;
				// Get the triangle id contained by the node
				iTrId = g_uPrimitivesId[g_uNodes[iNodeNum].iPrimPos];

				// Get the triangle data
				int offset = iTrId*3;
				float3 A = g_sVertices[g_sIndices[offset]].vfPosition.xyz;
				float3 B = g_sVertices[g_sIndices[offset+1]].vfPosition.xyz;
				float3 C = g_sVertices[g_sIndices[offset+2]].vfPosition.xyz;

				cIntersection = getIntersection(ray,A,B,C);
				// Search for an intersection:
				// 1. Avoid float-precision errors.
				// 2. Perform ray-triangle intersection test.
				// 3. Check if the new intersection is nearer to 
				// the camera than the current best intersection.
				if((ray.iTriangleId != iTrId)
					&& (RayTriangleTest(cIntersection)	)	
					&& (cIntersection.fT < bIntersection.fT))	
				{
					bIntersection = cIntersection;
					bIntersection.iTriangleId = iTrId;
					bIntersection.iRoot = iNodeNum;
				}
			}
			
			// If the stack is empty, the traversal ends
			if(iStackOffset == 0) break;					
			// Pop a new node from the stack
			iNodeNum = stack[--iStackOffset];
		}
		if(iStackOffset == 0) break;
	}

	bIntersection.iVisitedNodes = result;

	// return the best intersection found. If no intersection
	// was found, the intersection "contains" a triangle id = -1.
	return bIntersection;
}


// ------------------------------------------
// BVH intersection function
// ------------------------------------------
Intersection BVH_IntersectP(Ray ray)
{	
	// Initialize variables
	int iTrId = -1;
	float3 vfInvDir = float3(1.0f / ray.vfDirection);	
	
	int stack[35];
	int iStackOffset = 0;
	int iNodeNum = 0;
	int result = 0;
	
	// Initialize the current and best intersection.
	// They are empty at the beginning.
	Intersection cIntersection;
	Intersection bIntersection;
	bIntersection.iTriangleId = -1;
	bIntersection.fU = -1;
	bIntersection.fV = -1;
	bIntersection.fT = ray.fMaxT;
	bIntersection.iRoot = 0;
	bIntersection.iVisitedNodes = 0;
	
	[allow_uav_condition]
	while( true )
	{	
		// Perform ray-box intersection test
		float2 T = IntersectBox(ray.vfOrigin, vfInvDir, iNodeNum);
		result++;
		
		// If the ray does not intersect the box
		if ((T[0] > bIntersection.fT) || (T[1] < 0.0f))
		{
			// If the stack is empty, the traversal ends
			if(iStackOffset == 0) break;	
			// Pop a new node from the stack
			iNodeNum = stack[--iStackOffset];
		}
		// If the intersected box is a Leaf Node
		else if( g_uNodes[iNodeNum].iPrimCount > 0 )
		{
			[allow_uav_condition]
			for( int i = g_uNodes[iNodeNum].iPrimCount; i >= 0 ; --i )
			{
				result += 65536;
				// Get the triangle id contained by the node
				iTrId = g_uPrimitivesId[g_uNodes[iNodeNum].iPrimPos];

				// Get the triangle data
				int offset = iTrId*3;
				float3 A = g_sVertices[g_sIndices[offset]].vfPosition.xyz;
				float3 B = g_sVertices[g_sIndices[offset+1]].vfPosition.xyz;
				float3 C = g_sVertices[g_sIndices[offset+2]].vfPosition.xyz;

				cIntersection = getIntersection(ray,A,B,C);
				// Search for an intersection:
				// 1. Avoid float-precision errors.
				// 2. Perform ray-triangle intersection test.
				// 3. Check if the new intersection is nearer to 
				// the camera than the current best intersection.
				if((ray.iTriangleId != iTrId)
					&& (RayTriangleTest(cIntersection)	)	
					&& (cIntersection.fT < bIntersection.fT))	
				{
					bIntersection = cIntersection;
					bIntersection.iTriangleId = iTrId;
					bIntersection.iRoot = iNodeNum;
				}
			}
			
			// If the stack is empty, the traversal ends
			if(iStackOffset == 0) break;					
			// Pop a new node from the stack
			iNodeNum = stack[--iStackOffset];
		}
		// If the intersected box is an Inner Node
		else
		{
			// Depending on the ray direction and the split-axis,
			// the order of the children changes on the stack.
			int dirIsNeg[3] = { vfInvDir < 0 };
			// -g_uNodes[iNodeNum].iPrimCount is the split axis: 0-1-2 (x-y-z)
			const int aux = dirIsNeg[-g_uNodes[iNodeNum].iPrimCount];
			// aux replaces an if/else statement which improves traversal a little bit
			stack[iStackOffset++] = (iNodeNum+1)*aux + (1-aux)*g_uNodes[iNodeNum].iPrimPos;
			iNodeNum = g_uNodes[iNodeNum].iPrimPos*aux + (1-aux)*(iNodeNum+1);
		}
	}

	bIntersection.iVisitedNodes = result;

	// return the best intersection found. If no intersection
	// was found, the intersection "contains" a triangle id = -1.
	return bIntersection;
}
