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

#ifndef CUBICGRID_H
#define CUBICGRID_H

// CubicGrid.h*
#include "Memory.h"
#include "Geometry.h"
#include "AccelerationStructure.h"

#include "BBox.h"

// CubicGrid Forward Declarations
struct Voxel;

struct CubicGridPrimitiveInfo {
    CubicGridPrimitiveInfo() { }
    CubicGridPrimitiveInfo(int pn, const BBox &b)
		: primitiveNumber(pn), bounds(b){}

    int primitiveNumber;
	BBox bounds;
};

// Voxel Declarations
struct Voxel {
    // Voxel Public Methods
    uint32_t size() const { return primitives.size(); }
    Voxel() { }
    Voxel(CubicGridPrimitiveInfo op) {
        allCanIntersect = false;
        primitives.push_back(op);
    }
    void AddPrimitive(CubicGridPrimitiveInfo prim) {
        primitives.push_back(prim);
    }
    bool IntersectP(const Ray &ray, unsigned int &a_Candidates, TIntersection &Intersect,Point &_Start,Vector &_Direction, Primitive** a_Primitives);
private:
    vector<CubicGridPrimitiveInfo> primitives;
    bool allCanIntersect;
};

// CubicGrid Declarations
class CubicGrid : public AccelerationStructure 
{
public:
    // CubicGrid Public Methods
	CubicGrid(Primitive** a_Primitive = NULL, unsigned int a_NumPrimitives = 0);
    ~CubicGrid();
	TIntersection IntersectP(Ray &a_Ray);
	void PrintOutput(float &time);
	void Build();
	unsigned int				GetNumberOfElements() { return totalVoxels; }
	size_t						GetSizeOfElement() { return 0; }
	void*						GetPtrToElements() { return static_cast<void*>(&voxels); }
	unsigned int*				GetOrderedElementsIds() { return 0; }
	unsigned int				GetKeyboardId() { return 6;}
	BBox						WorldBound() { return bounds; }
private:
    // CubicGrid Private Methods
    int posToVoxel(const Point &P, int axis) const {
        int v = (int)((P[axis] - bounds.pMin[axis]) *
                          invWidth[axis]);
        return Clamp(v, 0, nVoxels[axis]-1);
    }
    float voxelToPos(int p, int axis) const {
        return bounds.pMin[axis] + p * width[axis];
    }
    inline int offset(int x, int y, int z) const {
        return z*nVoxels[0]*nVoxels[1] + y*nVoxels[0] + x;
    }

    // CubicGrid Private Data
    vector<CubicGridPrimitiveInfo > primitives;
    int nVoxels[3];
    BBox bounds;
    Vector width, invWidth;
    Voxel **voxels;
    MemoryArena voxelArena;
	int	totalVoxels;
};
#endif