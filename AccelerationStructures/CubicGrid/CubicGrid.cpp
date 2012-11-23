/*
 * pbrt source code Copyright(c) 1998-2004 Matt Pharr and Greg Humphreys
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 * (See file License.txt for complete license)
 */

// --------------------------------------------------------------------	//
// This code was modified by the authors of the demo. The original		//
// PBRT code is available at https://github.com/mmp/pbrt-v2. Basically, //
// we removed all STL-based implementation and it was merged with		//
// our current framework.												//
// --------------------------------------------------------------------	//

#include "CubicGrid.h"

inline int Floor2Int(float val) {
    return (int)floorf(val);
}

inline int Round2Int(float val) {
    return Floor2Int(val + 0.5f);
}

// CubicGrid Method Definitions
CubicGrid::CubicGrid(Primitive** pPrimitives, unsigned int uiNumPrimitives) 
{   
	m_pPrimitives = pPrimitives;
	m_uiNumPrimitives = uiNumPrimitives;
	m_sName = "Cubic Grid";	
}

CubicGrid::~CubicGrid() 
{
    for (int i = 0; i < nVoxels[0]*nVoxels[1]*nVoxels[2]; ++i)
        if (voxels[i]) voxels[i]->~Voxel();
    FreeAligned(voxels);
}

void CubicGrid::Build() 
{
	// Compute bounds and choose grid resolution
	primitives.reserve(m_uiNumPrimitives);
	for (unsigned int i = 0; i < m_uiNumPrimitives; ++i) 
	{ 
		Point Vertex1(m_pPrimitives[i]->GetVertex(0)->Pos);
		Point Vertex2(m_pPrimitives[i]->GetVertex(1)->Pos);
		Point Vertex3(m_pPrimitives[i]->GetVertex(2)->Pos);
		BBox bbox = Union(BBox(Vertex1, Vertex2),Vertex3);

		bounds = Union(bounds, bbox);
        primitives.push_back(CubicGridPrimitiveInfo(i, bbox));
    }

    Vector delta = bounds.pMax - bounds.pMin;

    // Find _voxelsPerUnitDist_ for grid
    int maxAxis = bounds.MaximumExtent();
    float invMaxWidth = 1.f / delta[maxAxis];
    Assert(invMaxWidth > 0.f);
    float cubeRoot = 3.f * powf(float(primitives.size()), 1.f/3.f);
    float voxelsPerUnitDist = cubeRoot * invMaxWidth;
    for (int axis = 0; axis < 3; ++axis) {
        nVoxels[axis] = Round2Int(delta[axis] * voxelsPerUnitDist);
        nVoxels[axis] = Clamp(nVoxels[axis], 1, 128);
    }

    // Compute voxel widths and allocate voxels
    for (int axis = 0; axis < 3; ++axis) {
        width[axis] = delta[axis] / nVoxels[axis];
        invWidth[axis] = (width[axis] == 0.f) ? 0.f : 1.f / width[axis];
    }
    totalVoxels = nVoxels[0] * nVoxels[1] * nVoxels[2];
    voxels = AllocAligned<Voxel *>(totalVoxels);
    memset(voxels, 0, totalVoxels * sizeof(Voxel *));



    // Add primitives to grid voxels
    for (uint32_t i = 0; i < primitives.size(); ++i) {
        // Find voxel extent of primitive
		BBox pb = primitives[i].bounds;
        int vmin[3], vmax[3];
        for (int axis = 0; axis < 3; ++axis) {
            vmin[axis] = posToVoxel(pb.pMin, axis);
            vmax[axis] = posToVoxel(pb.pMax, axis);
        }

        // Add primitive to overlapping voxels
        for (int z = vmin[2]; z <= vmax[2]; ++z)
            for (int y = vmin[1]; y <= vmax[1]; ++y)
                for (int x = vmin[0]; x <= vmax[0]; ++x) {
                    int o = offset(x, y, z);
                    if (!voxels[o]) {
                        // Allocate new voxel and store primitive in it
                        voxels[o] = voxelArena.Alloc<Voxel>();
                        *voxels[o] = Voxel(primitives[i]);
                    }
                    else {
                        // Add primitive to already-allocated voxel
                        voxels[o]->AddPrimitive(primitives[i]);
                    }
                }
    }
}

TIntersection CubicGrid::IntersectP(Ray &a_Ray)
{
    // Check ray against overall grid bounds
    float rayT;
	TIntersection Intersect(-1,a_Ray.maxt);	

	TIntersection test;	
	bool result = false;

    if (bounds.Inside(a_Ray(a_Ray.mint)))
        rayT = a_Ray.mint;
    else if (!bounds.IntersectP(a_Ray, &rayT))
    {
        return Intersect;
    }

    Point gridIntersect = a_Ray(rayT);

    // Set up 3D DDA for ray
    float NextCrossingT[3], DeltaT[3];
    int Step[3], Out[3], Pos[3];
    for (int axis = 0; axis < 3; ++axis) {
        // Compute current voxel for axis
        Pos[axis] = posToVoxel(gridIntersect, axis);
        if (a_Ray.d[axis] >= 0) {
            // Handle ray with positive direction for voxel stepping
            NextCrossingT[axis] = rayT +
                (voxelToPos(Pos[axis]+1, axis) - gridIntersect[axis]) / a_Ray.d[axis];
            DeltaT[axis] = width[axis] / a_Ray.d[axis];
            Step[axis] = 1;
            Out[axis] = nVoxels[axis];
        }
        else {
            // Handle ray with negative direction for voxel stepping
            NextCrossingT[axis] = rayT +
                (voxelToPos(Pos[axis], axis) - gridIntersect[axis]) / a_Ray.d[axis];
            DeltaT[axis] = -width[axis] / a_Ray.d[axis];
            Step[axis] = -1;
            Out[axis] = -1;
        }
    }

    // Walk grid for shadow ray
    while (!result) {
        int o = offset(Pos[0], Pos[1], Pos[2]);
        Voxel *voxel = voxels[o];
        /*if (voxel && voxel->IntersectP(ray, Intersect, candidates,Start, Direction,g_Object, lock))
		{
		 return Intersect;
		}*/
		if(voxel) voxel->IntersectP(a_Ray, m_uiCandidates, Intersect, a_Ray.o, a_Ray.d, m_pPrimitives);
        // Advance to next voxel

        // Find _stepAxis_ for stepping to next voxel
        int bits = ((NextCrossingT[0] < NextCrossingT[1]) << 2) +
                   ((NextCrossingT[0] < NextCrossingT[2]) << 1) +
                   ((NextCrossingT[1] < NextCrossingT[2]));
        const int cmpToAxis[8] = { 2, 1, 2, 1, 2, 2, 0, 0 };
        int stepAxis = cmpToAxis[bits];
        if (a_Ray.maxt < NextCrossingT[stepAxis])
            break;
        Pos[stepAxis] += Step[stepAxis];
        if (Pos[stepAxis] == Out[stepAxis])
            break;
        NextCrossingT[stepAxis] += DeltaT[stepAxis];
    }
    return Intersect;
}

void CubicGrid::PrintOutput(float &tiempototal)
{
	printf("Grid: %d Voxels * %d Bytes per Voxel = %.2f MB \n",totalVoxels, sizeof(Voxel), (float)(totalVoxels*sizeof(Voxel))/(float)(1024*1024));
}

bool Voxel::IntersectP(const Ray &ray, unsigned int &candidates, TIntersection &Intersect, Point &Start,Vector &Direction, Primitive** a_Primitives) 
{
	int TriangleID; 
	TIntersection test;	
	bool result = false;
	
	//candidates++;
    for (uint32_t i = 0; i < primitives.size(); ++i) {
        CubicGridPrimitiveInfo &prim = primitives[i];
		//candidates++;
		if (prim.bounds.IntersectP(ray)) {
			TriangleID = prim.primitiveNumber;
            candidates++;
		    if (RayTriangleTest(Start,Direction,test,TriangleID,a_Primitives) && test.t<Intersect.t) {
			  Intersect.t = test.t;
			  Intersect.u = test.u;
			  Intersect.v = test.v;
			  Intersect.IDTr = TriangleID;
			  result = true;
			}
          } 
    }
    return result;
}