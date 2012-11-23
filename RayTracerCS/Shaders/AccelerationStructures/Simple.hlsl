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

// -----------------------------------------------------------
// Simple ray intersection (No Acceleration Structure)
// -----------------------------------------------------------
Intersection IntersectP(Ray ray)
{
	Intersection cIntersection;
	Intersection bIntersection;
	bIntersection.iTriangleId = -1;
	bIntersection.fT = 10000.f;
	bIntersection.fU = -1;
	bIntersection.fV = -1;

	const int iNumPrimitives = 10;

	for(int i = 0; i < iNumPrimitives; ++i)
	{
		unsigned int offset = i*3;
		float3 A = g_sVertices[g_sIndices[offset]].vfPosition;
		float3 B = g_sVertices[g_sIndices[offset+1]].vfPosition;
		float3 C = g_sVertices[g_sIndices[offset+2]].vfPosition;
		
		cIntersection = getIntersection(ray,A,B,C);
		if(ray.iTriangleId != i && 
			RayTriangleTest(cIntersection) && 
			cIntersection.fT < bIntersection.fT)
		{
			bIntersection = cIntersection;
			bIntersection.iTriangleId = i;
		}
	}

	return bIntersection;
}