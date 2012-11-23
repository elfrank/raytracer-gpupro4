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

#ifndef KDTree_H
#define KDTree_H

// KDTree.h*
#include <stdlib.h>

#ifdef WINDOWS
#include <windows.h>
#elif defined(LINUX)
#include <DataTypes.h>
#endif

#include "Memory.h"
#include "Geometry.h"
#include "AccelerationStructure.h"

#include "BBox.h"

// KDTree Declarations
struct KDTreePrimitiveInfo;

// KDTree Local Declarations
struct KdNode {
    // KdNode Methods
    void initLeaf(uint32_t *primNums, int np, MemoryArena &arena);
    void initInterior(uint32_t axis, uint32_t ac, float s) {
        split = s;
        flags = axis;
        aboveChild |= (ac << 2);
    }
    float SplitPos() const { return split; }
    uint32_t nPrimitives() const { return nPrims >> 2; }
    uint32_t SplitAxis() const { return flags & 3; }
    bool IsLeaf() const { return (flags & 3) == 3; }
    uint32_t AboveChild() const { return aboveChild >> 2; }
    union {
        float split;            // Interior
        uint32_t onePrimitive;  // Leaf
        uint32_t *primitives;   // Leaf
    };
private:
    union {
        uint32_t flags;         // Both
        uint32_t nPrims;        // Leaf
        uint32_t aboveChild;    // Interior
    };
};


struct BoundEdge {
    // BoundEdge Public Methods
    BoundEdge() { }
    BoundEdge(float tt, int pn, bool starting) {
        t = tt;
        primNum = pn;
        type = starting ? START : END;
    }
    bool operator<(const BoundEdge &e) const {
        if (t == e.t)
            return (int)type < (int)e.type;
        else return t < e.t;
    }
    float t;
    int primNum;
    enum { START, END } type;
};


class KDTree : public AccelerationStructure
{
public:
    // KDTree Public Methods
    KDTree(Primitive** a_Primitive = NULL, unsigned int a_NumPrimitives = 0, 
                int icost = 80, int scost = 1,  float ebonus = 0.5f, int maxp = 1,
                int maxDepth = -1);

	__int64								g_candidates;
    BBox WorldBound() { return bounds; }
	void Rebuild(void);
	void PrintOutput(float &time);
    ~KDTree();
  	TIntersection IntersectP(Ray &a_Ray);
	void Build();
	unsigned int				GetNumberOfElements() { return totalNodes; }
	size_t						GetSizeOfElement() { return sizeof(KdNode); }
	void*						GetPtrToElements() { return static_cast<void*>(nodes); }
	unsigned int*				GetOrderedElementsIds() { return 0; }
	unsigned int				GetKeyboardId() { return 7;}
private:
    // KDTree Private Methods
    void BuildTree(int nodeNum, const BBox &bounds,
        const vector<BBox> &primBounds, uint32_t *primNums, int nprims, int depth,
        BoundEdge *edges[3], uint32_t *prims0, uint32_t *prims1, int badRefines = 0);

    // KDTree Private Data
    int isectCost, traversalCost, maxPrims, maxDepth;
    float emptyBonus;
    vector<KDTreePrimitiveInfo > primitives;
    KdNode *nodes;
    int nAllocedNodes, nextFreeNode;
    BBox bounds;
    MemoryArena arena;
	int totalNodes;
};

struct KDTreePrimitiveInfo {
    KDTreePrimitiveInfo() { }
    KDTreePrimitiveInfo(int pn, const BBox &b)
		: primitiveNumber(pn), bounds(b){}

    int primitiveNumber;
	BBox bounds;
};

struct KdToDo {
    const KdNode *node;
    float tmin, maxT;
};

#endif 
