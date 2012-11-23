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

#ifndef _BIH_H_
#define _BIH_H_

// bvh.h*
#include <stdlib.h>
#ifdef WINDOWS
#include <windows.h>
#elif defined(LINUX)
#include <DataTypes.h>
#endif
#include "Memory.h"
#include "Geometry.h"
#include "BBox.h"
#include "AccelerationStructure.h"

#include "BBox.h"

class BBox;

// BIH Local Declarations
// BIH Local Declarations
struct BIHPrimitiveInfo 
{
    BIHPrimitiveInfo() { }
    BIHPrimitiveInfo(int pn, const BBox &b)
        : primitiveNumber(pn), bounds(b) {
        centroid = .5f * b.pMin + .5f * b.pMax;
    }

	TIntersection intersect()
	{
		TIntersection hit(-1,100000000);

		return hit;
	}

    int primitiveNumber;
    Point centroid;
    BBox bounds;
};

struct SplitBIH
{
	float position;
	uint32_t axis;

	SplitBIH() { position = 0.f, axis = -1; }
	SplitBIH(float p, uint32_t a) : position(p), axis(a) {}
};

struct BIHNode 
{
    // BIHBuildNode Public Methods
    BIHNode() { children[0] = children[1] = NULL; left = right = -1; }

	bool isLeaf() const { return children[0] == 0 && children[1] == 0;}
    void InitLeaf(uint32_t l, uint32_t r) 
	{
		children[0] = children[1] = 0;
		clip[0] = clip[1] = 0;
		left = l;
		right = r;
		splitAxis = SplitBIH();
    }

	BIHNode *children[2];
	float clip[2];
	uint32_t left,right;
	SplitBIH splitAxis;
};

struct CompareToMidBIH 
{
    CompareToMidBIH(int d, float m) { dim = d; mid = m; }
    int dim;
    float mid;
    bool operator()(const BIHPrimitiveInfo &a) const 
	{
        return a.centroid[dim] < mid;
    }
};

struct ComparePointsBIH 
{
    ComparePointsBIH(int d) { dim = d; }
    int dim;
    bool operator()(const BIHPrimitiveInfo &a, const BIHPrimitiveInfo &b) const 
	{
        return a.centroid[dim] < b.centroid[dim];
    }
};

struct LinearBIHNode 
{
	union
	{
		uint32_t primitivesOffset;		// Leaf
		uint32_t secondChildOffset;		// Interior
	};
	
	union
	{
		uint32_t nPrimitives;			// Leaf
		float mClip[2];					// Interior
	};
	
	bool isLeaf() const { return (primitivesOffset & 0x03) == 0x03; }
	uint32_t getSplitAxis() const { return (primitivesOffset & 0x03); }
	uint32_t getPrimitivesOffset() { return primitivesOffset>>3; }
	uint32_t getSecondChildOffset() { return secondChildOffset>>3; }
};

struct BIHToDo {
    //const LinearBIHNode *node;
    uint32_t nodeNum;
    float tMinS, tMaxS;
};

// BIH Declarations
class BIH : public AccelerationStructure
{
public:
	
    // BIH Public Methods
	BIH(Primitive** a_Primitive = NULL, unsigned int a_NumPrimitives = 0, uint32_t maxPrims = 1);
    ~BIH();

	TIntersection				IntersectP(Ray &a_Ray);
	void						PrintOutput(float &time);
	size_t						GetSizeOfElement() { return sizeof(LinearBIHNode); }
	void*						GetPtrToElements() { return static_cast<void*>(nodes); }
	unsigned int				GetNumberOfElements() { return totalNodes; }
	unsigned int*				GetOrderedElementsIds() { return 0; }
	unsigned int				GetKeyboardId() { return 8;}
	BBox						WorldBound() { return BBox(); }
private:
    // BIH Private Methods
    uint32_t					FlattenBIHTree(BIHNode *node, uint32_t *offset);
    uint32_t					maxPrimsInNode;
	uint32_t					totalNodes;
	vector<BIHPrimitiveInfo>	primitives;
	BIHNode						*root;
	LinearBIHNode				*nodes;
	BIHNode*					InitNode(MemoryArena &buildArena, const BBox& bbox, int left, int right, int depth, int singleDepth, uint32_t *totalNodes);
	TIntersection				Traverse(const BIHNode* node, const Ray &ray, const float& tMin, const float& tMax);
	TIntersection				LeafIntersect(Ray& ray, const float& tMin, const float& tMax);
	BIHNode*					BuildPatch(BIHNode* node, BIHNode* tmpNode);
	void						DestroyPatch(BIHNode* node);

	bool						IsLeaf(BIHNode* node) const;
	bool						IsSingle(BIHNode* node) const;
	void						Build();
	
	
};

#endif // BIH_H