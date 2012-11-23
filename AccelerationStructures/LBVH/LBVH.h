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

#ifndef BINARYOCTREE_H
#define BINARYOCTREE_H

#include "Geometry.h"
#include "Ray.h"
#include "AccelerationStructure.h"
#include "BBox.h"

using namespace std;

struct LBVHNode
{
	Vector3 minCoord,maxCoord;
	int PrimCount, PrimPos;

	void PrintString()
	{
		//if(PrimPos < 0)
		{
			printf("PrimPos=%d | PrimCount=%d | axis=%d | isleaf=%d\n", PrimPos, PrimCount>>3,PrimCount&3,(PrimCount&4)>>2 );
			printf("BoxMin(%f,%f,%f), BoxMax(%f,%f,%f)\n", minCoord.x,minCoord.y,minCoord.z,maxCoord.x,maxCoord.y,maxCoord.z);
		}
	}
};

static inline void LBVH_IntersectP(const BBox &bounds, const Ray &ray,const Vector &invDir, const int dirIsNeg[3] ,float &tmin,float &tmax) 
{
    // Check for ray intersection against $x$ and $y$ slabs
    tmin =  (bounds[  dirIsNeg[0]].x - ray.o.x) * invDir.x;
    tmax =  (bounds[1-dirIsNeg[0]].x - ray.o.x) * invDir.x;
    float tymin = (bounds[  dirIsNeg[1]].y - ray.o.y) * invDir.y;
    float tymax = (bounds[1-dirIsNeg[1]].y - ray.o.y) * invDir.y;

    if ((tmin > tymax) || (tymin > tmax))
	{
		tmin = tmax = -1.f;
		return;
	}

	tmin=max(tmin,tymin);
	tmax=min(tmax,tymax);

    // Check for ray intersection against $z$ slab
    float tzmin = (bounds[  dirIsNeg[2]].z - ray.o.z) * invDir.z;
    float tzmax = (bounds[1-dirIsNeg[2]].z - ray.o.z) * invDir.z;
    if ((tmin > tzmax) || (tzmin > tmax))
	{
		tmin = tmax = -1.f;
		return;
	}

	tmin=max(tmin,tzmin);
	tmax=min(tmax,tzmax);
}

class LBVH : public AccelerationStructure
{
private:
	int m_iNumNodes;
	LBVHNode* m_pNodes;
	int *m_pPrimitivesIds;
	MortonCode *m_pPrimitivesMortonId;
	int OccupiedNodes;
	int occupiedLeafs;
	
	// temporary
	Vector3 *gMinVertices;
	Vector3 *gMaxVertices;
	Point *gCentroid;

	TIntersection intersectPppp(Ray &a_Ray);
	TIntersection intersectPppn(Ray &a_Ray);
	TIntersection intersectPpnp(Ray &a_Ray);
	TIntersection intersectPpnn(Ray &a_Ray);
	TIntersection intersectPnpp(Ray &a_Ray);
	TIntersection intersectPnpn(Ray &a_Ray);
	TIntersection intersectPnnp(Ray &a_Ray);
	TIntersection intersectPnnn(Ray &a_Ray);

	void Build();

	void build_sah(int position,int count,unsigned __int64 bit,int node, int axis);
	void build_recursive_nlog2n(MortonCode *ordered_list, const unsigned int &NumPrim);
	void build_iterative(MortonCode *ordered_list, const unsigned int &NUM_BITS, const unsigned int &SIZE);
	//void build_recursive(int position,int count,unsigned __int64 bit,int node, int axis);
	void build_recursive(int position,int count,unsigned int bit,int node, int axis);

public:

	LBVH(Primitive** a_Primitive = NULL, unsigned int a_NumPrimitives = 0, int n = 1<<21);
	TIntersection IntersectP(Ray &a_Ray);
	void PrintOutput(float &totaltime);
	
	//int* getPrimitives() { return m_pPrimitivesIds; }
	unsigned int				GetNumberOfElements() { return m_iNumNodes; }
	void						setm_iNumNodes(int &n) { m_iNumNodes = n; }
	size_t						GetSizeOfElement() { return sizeof(LBVHNode); }
	void*						GetPtrToElements() { return static_cast<void*>(m_pNodes); }
	unsigned int*				GetOrderedElementsIds() { return reinterpret_cast<unsigned int*>(m_pPrimitivesIds); }
	unsigned int				GetKeyboardId() { return 9;}
	BBox						WorldBound() { return m_pNodes ? BBox(m_pNodes[1].minCoord, m_pNodes[1].maxCoord ) : BBox(); }

	~LBVH();
};

#endif