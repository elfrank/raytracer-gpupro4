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
// Ray-Box Intersection Test
// ------------------------------------------
float2 IntersectBox(float3 Start, float3 invDir, unsigned int nodeNum)
{
	float2 T;
	float3 DiffMax = g_uNodes[nodeNum].vfMax-Start;
	float3 DiffMin = g_uNodes[nodeNum].vfMin-Start;
	DiffMax *= invDir;
	DiffMin *= invDir;

	T[0] = min(DiffMin.x,DiffMax.x);
	T[1] = max(DiffMin.x,DiffMax.x);

	T[0] = max(T[0],min(DiffMin.y,DiffMax.y));
	T[1] = min(T[1],max(DiffMin.y,DiffMax.y));

	T[0] = max(T[0],min(DiffMin.z,DiffMax.z));
	T[1] = min(T[1],max(DiffMin.z,DiffMax.z));

	//empty interval
	if (T[0] > T[1])
	{
		T[0] = T[1] = -1.0f;
	}

	return T;
}

// ------------------------------------------
// Gets the current ray intersection
// ------------------------------------------
Intersection getIntersection(Ray ray, float3 A, float3 B, float3 C)
{
	Intersection intersection;
	intersection.iTriangleId = -1;
	
	float3 P, T, Q;
	float3 E1 = B-A;
	float3 E2 = C-A;
	P = cross(ray.vfDirection,E2);
	float det = 1.0f/dot(E1,P);
	T = ray.vfOrigin - A;
	intersection.fU = dot(T,P) * det;
	Q = cross(T,E1);
	intersection.fV = dot(ray.vfDirection,Q)*det;
	intersection.fT = dot(E2,Q)*det;
	
	return intersection;
}

// ------------------------------------------
// Ray-Triangle intersection test
// ------------------------------------------
bool RayTriangleTest(Intersection intersection)
{
	return (
		(intersection.fU >= 0.0f)
		//&& (intersection.fU <= 1.0f)
		&& (intersection.fV >= 0.0f)
		&& ((intersection.fU+intersection.fV) <= 1.0f)
		&& (intersection.fT >= 0.0f)
	);
}

// ------------------------------------------
// Calculate UVs for a given triangle
// ------------------------------------------
float2 computeUVs(int iTriangleId, Intersection intersection)
{
	int offset = iTriangleId*3;
	float2 A,B,C;

	A.xy = g_sVertices[g_sIndices[offset]].vfUvs;
	B.xy = g_sVertices[g_sIndices[offset+1]].vfUvs;
	C.xy = g_sVertices[g_sIndices[offset+2]].vfUvs;

	return intersection.fU* B.xy +intersection.fV * C.xy + (1.0f-(intersection.fU + intersection.fV))*A;
}

// ------------------------------------------
// Calculate barycentric coordinates
// ------------------------------------------
float3 Vec3BaryCentric(float3 V1, float3 V2, float3 V3, float2 UV)
{
	float3 fOut = 0.f;
	float t = 1.f - (UV.x+UV.y);

	fOut[0] = V1[0]*t + V2[0]*UV.x + V3[0]*UV.y;
	fOut[1] = V1[1]*t + V2[1]*UV.x + V3[1]*UV.y;
	fOut[2] = V1[2]*t + V2[2]*UV.x + V3[2]*UV.y;

	return fOut;
}
