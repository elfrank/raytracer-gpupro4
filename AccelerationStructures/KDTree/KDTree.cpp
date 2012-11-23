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

#include "KDTree.h"

inline float Log2(float x) {
    static float invLog2 = 1.f / logf(2.f);
    return logf(x) * invLog2;
}


inline int Floor2Int(float val) {
    return (int)floorf(val);
}

inline int Log2Int(float v) {
    return Floor2Int(Log2(v));
}



inline int Round2Int(float val) {
    return Floor2Int(val + 0.5f);
}
// KDTree Method Definitions
KDTree::KDTree(Primitive** a_Primitives, unsigned int a_NumPrimitives, int icost, int tcost, float ebonus, int maxp, int md)
    : isectCost(icost), traversalCost(tcost), maxPrims(maxp), maxDepth(md),
      emptyBonus(ebonus) 
{
	g_candidates = 0;
	m_sName = "KDTree";
	m_pPrimitives = a_Primitives;
	m_uiNumPrimitives = a_NumPrimitives;

// Build kd-tree
    nextFreeNode = nAllocedNodes = 0;
    if (maxDepth <= 0)
		maxDepth = Round2Int(8 + 1.3f * Log2Int(float(m_uiNumPrimitives)));
	
	totalNodes = 0;
}

void KDTree::Build()
{
	// Compute bounds for kd-tree construction
    vector<BBox> primBounds;
	primBounds.reserve(m_uiNumPrimitives);
	primitives.reserve(m_uiNumPrimitives);

	for (unsigned int i = 0; i < m_uiNumPrimitives; ++i) 
	{ 
		Point Vertex1(m_pPrimitives[i]->GetVertex(0)->Pos);
		Point Vertex2(m_pPrimitives[i]->GetVertex(1)->Pos);
		Point Vertex3(m_pPrimitives[i]->GetVertex(2)->Pos);
		BBox bbox = Union(BBox(Vertex1, Vertex2),Vertex3);

		bounds = Union(bounds, bbox);
        primBounds.push_back(bbox);
		primitives.push_back(KDTreePrimitiveInfo(i, bbox));
    }

    // Allocate working memory for kd-tree construction
    BoundEdge *edges[3];
    for (int i = 0; i < 3; ++i)
        edges[i] = new BoundEdge[2*primitives.size()];
    uint32_t *prims0 = new uint32_t[primitives.size()];
    uint32_t *prims1 = new uint32_t[(maxDepth+1) * primitives.size()];

    // Initialize _primNums_ for kd-tree construction
    uint32_t *primNums = new uint32_t[primitives.size()];
    for (uint32_t i = 0; i < primitives.size(); ++i)
        primNums[i] = i;

    // Start recursive construction of kd-tree
    BuildTree(0, bounds, primBounds, primNums, primitives.size(),
              maxDepth, edges, prims0, prims1);

	// Free working memory for kd-tree construction
    delete[] primNums;
    for (int i = 0; i < 3; ++i)
        delete[] edges[i];
    delete[] prims0;
    delete[] prims1;
}

void KdNode::initLeaf(uint32_t *primNums, int np,
                           MemoryArena &arena) {
    flags = 3;
    nPrims |= (np << 2);
    // Store primitive ids for leaf node
    if (np == 0)
        onePrimitive = 0;
    else if (np == 1)
        onePrimitive = primNums[0];
    else {
        primitives = arena.Alloc<uint32_t>(np);
        for (int i = 0; i < np; ++i)
            primitives[i] = primNums[i];
	
    }
}


KDTree::~KDTree() {
    FreeAligned(nodes);
}


void KDTree::BuildTree(int nodeNum, const BBox &nodeBounds,
        const vector<BBox> &allPrimBounds, uint32_t *primNums,
        int nPrimitives, int depth, BoundEdge *edges[3],
        uint32_t *prims0, uint32_t *prims1, int badRefines) {
    Assert(nodeNum == nextFreeNode);
	totalNodes++;
    // Get next free node from _nodes_ array
    if (nextFreeNode == nAllocedNodes) 
	{
        int nAlloc = max(2 * nAllocedNodes, 512);
        KdNode *n = AllocAligned<KdNode>(nAlloc);
        if (nAllocedNodes > 0) 
		{
            memcpy(n, nodes, nAllocedNodes * sizeof(KdNode));
            FreeAligned(nodes);
        }
        nodes = n;
        nAllocedNodes = nAlloc;
    }
    ++nextFreeNode;

    // Initialize leaf node if termination criteria met

	if (nPrimitives <= maxPrims || depth == 0) {
       
        nodes[nodeNum].initLeaf(primNums, nPrimitives, arena);
        return;
    }

    // Initialize interior node and continue recursion

    // Choose split axis position for interior node
    int bestAxis = -1, bestOffset = -1;
    float bestCost = INFINITY;
    float oldCost = isectCost * float(nPrimitives);
    float totalSA = nodeBounds.SurfaceArea();
    float invTotalSA = 1.f / totalSA;
    Vector d = nodeBounds.pMax - nodeBounds.pMin;

    // Choose which axis to split along
    uint32_t axis = nodeBounds.MaximumExtent();
    int retries = 0;
    bool retrySplit = true;
	while (retrySplit) {
		retrySplit=false;
		// Initialize edges for _axis_
		for (int i = 0; i < nPrimitives; ++i) {
			int pn = primNums[i];
		    const BBox &bbox = allPrimBounds[pn];
		 edges[axis][2*i] =   BoundEdge(bbox.pMin[axis], pn, true);
		 edges[axis][2*i+1] = BoundEdge(bbox.pMax[axis], pn, false);
		}
		sort(&edges[axis][0], &edges[axis][2*nPrimitives]);

		// Compute cost of all splits for _axis_ to find best
		int nBelow = 0, nAbove = nPrimitives;
		for (int i = 0; i < 2*nPrimitives; ++i) {
		 if (edges[axis][i].type == BoundEdge::END) --nAbove;
		 float edget = edges[axis][i].t;
		 if (edget > nodeBounds.pMin[axis] &&
		     edget < nodeBounds.pMax[axis]) {
				// Compute cost for split at _i_th edge
				uint32_t otherAxis0 = (axis + 1) % 3, otherAxis1 = (axis + 2) % 3;
				float belowSA = 2 * (d[otherAxis0] * d[otherAxis1] +
                                 (edget - nodeBounds.pMin[axis]) *
                                 (d[otherAxis0] + d[otherAxis1]));
				float aboveSA = 2 * (d[otherAxis0] * d[otherAxis1] +
                                 (nodeBounds.pMax[axis] - edget) *
                                 (d[otherAxis0] + d[otherAxis1]));
				float pBelow = belowSA * invTotalSA;
				float pAbove = aboveSA * invTotalSA;
				float eb = (nAbove == 0 || nBelow == 0) ? emptyBonus : 0.f;
				float cost = traversalCost +
                         isectCost * (1.f - eb) * (pBelow * nBelow + pAbove * nAbove);

				// Update best split if this is lowest cost so far
				if (cost < bestCost)  {
					bestCost = cost;
					bestAxis = axis;
					bestOffset = i;
				}
         }
         if (edges[axis][i].type == BoundEdge::START) ++nBelow;
		}
		Assert(nBelow == nPrimitives && nAbove == 0);

		// Create leaf if no good splits were found
		 if (bestAxis == -1 && retries < 2) {
			++retries;
			axis = (axis+1) % 3;
			retrySplit=true;
		}
	}
    if (bestCost > oldCost) ++badRefines;
    if ((bestCost > 4.f * oldCost && nPrimitives < 16) ||
        bestAxis == -1 || badRefines == 3) {
        nodes[nodeNum].initLeaf(primNums, nPrimitives, arena);
        return;
    }

    // Classify primitives with respect to split
    int n0 = 0, n1 = 0;
    for (int i = 0; i < bestOffset; ++i)
        if (edges[bestAxis][i].type == BoundEdge::START)
            prims0[n0++] = edges[bestAxis][i].primNum;
    for (int i = bestOffset+1; i < 2*nPrimitives; ++i)
        if (edges[bestAxis][i].type == BoundEdge::END)
            prims1[n1++] = edges[bestAxis][i].primNum;

    // Recursively initialize children nodes
    float tsplit = edges[bestAxis][bestOffset].t;
   
    BBox bounds0 = nodeBounds, bounds1 = nodeBounds;
    bounds0.pMax[bestAxis] = bounds1.pMin[bestAxis] = tsplit;
    BuildTree(nodeNum+1, bounds0,
              allPrimBounds, prims0, n0, depth-1, edges,
              prims0, prims1 + nPrimitives, badRefines);
    uint32_t aboveChild = nextFreeNode;
    nodes[nodeNum].initInterior(bestAxis, aboveChild, tsplit);
    BuildTree(aboveChild, bounds1, allPrimBounds, prims1, n1,
              depth-1, edges, prims0, prims1 + nPrimitives, badRefines);
}



TIntersection KDTree::IntersectP(Ray &a_Ray)
{
   
    // Compute initial parametric range of ray inside kd-tree extent
    float tmin;
	TIntersection Intersect(-1,a_Ray.maxt);	
	
    if (!bounds.IntersectP(a_Ray, &tmin, &a_Ray.maxt))
    {
       return Intersect;
    }

   
	// Prepare to traverse kd-tree for ray
    Vector invDir(1.f/a_Ray.d.x, 1.f/a_Ray.d.y, 1.f/a_Ray.d.z);

	int TriangleID; 
	TIntersection test;	
	bool result = false;

#define MAX_TODO 64
    KdToDo todo[MAX_TODO];
    int todoPos = 0;
    const KdNode *node = &nodes[0];
    while (node != NULL) {
        if (node->IsLeaf()) {
            // Check for shadow ray intersections inside leaf node
            uint32_t nPrimitives = node->nPrimitives();
            if (nPrimitives == 1) {
                const KDTreePrimitiveInfo &prim = primitives[node->onePrimitive];
               
                //m_Candidates++;
				if (prim.bounds.IntersectP(a_Ray)){
						TriangleID = prim.primitiveNumber;
						//candidates++;
						if (RayTriangleTest(a_Ray.o,a_Ray.d,test,TriangleID,m_pPrimitives) && test.t<Intersect.t) {
							Intersect.t = test.t;
								Intersect.u = test.u;
								Intersect.v = test.v;
								Intersect.IDTr = TriangleID;
								result = true;
								 break;
						}
					}
				//break;
            }
            else {
                uint32_t *prims = node->primitives;
				bool tmp = false;
                for (uint32_t i = 0; i < nPrimitives; ++i) {
                    const KDTreePrimitiveInfo &prim = primitives[prims[i]];
					//m_Candidates++;
					if (prim.bounds.IntersectP(a_Ray)){
						TriangleID = prim.primitiveNumber;
						//candidates++;
						if (RayTriangleTest(a_Ray.o,a_Ray.d,test,TriangleID,m_pPrimitives) && test.t<Intersect.t) {
								Intersect.t = test.t;
								Intersect.u = test.u;
								Intersect.v = test.v;
								Intersect.IDTr = TriangleID;
								result = true;
								tmp = true;
						}
						
					}
                }
				if(tmp) break;
            }

            // Grab next node to process from todo list
            if (todoPos > 0) {
                --todoPos;
                node = todo[todoPos].node;
                tmin = todo[todoPos].tmin;
                a_Ray.maxt = todo[todoPos].maxT;
            }
            else
                break;
        }
        else {
            
            // Process kd-tree interior node

            // Compute parametric distance along ray to split plane
            int axis = node->SplitAxis();
            float tplane = (node->SplitPos() - a_Ray.o[axis]) * invDir[axis];

            // Get node children pointers for ray
            const KdNode *firstChild, *secondChild;
            int belowFirst = (a_Ray.o[axis] <  node->SplitPos()) ||
                             (a_Ray.o[axis] == node->SplitPos() && a_Ray.d[axis] >= 0);
            if (belowFirst) {
                firstChild = node + 1;
                secondChild = &nodes[node->AboveChild()];
            }
            else {
                firstChild = &nodes[node->AboveChild()];
                secondChild = node + 1;
            }

            // Advance to next child node, possibly enqueue other child
            if (tplane > a_Ray.maxt || tplane <= 0)
                node = firstChild;
            else if (tplane < tmin)
                node = secondChild;
            else {
                // Enqueue _secondChild_ in todo list
                todo[todoPos].node = secondChild;
                todo[todoPos].tmin = tplane;
                todo[todoPos].maxT = a_Ray.maxt;
                ++todoPos;
                node = firstChild;
                a_Ray.maxt = tplane;
            }
        }
    }
 
 return Intersect;
}


 void KDTree::Rebuild()
{

}

void KDTree::PrintOutput(float &tiempototal)
{
	printf("KDTree: %d Nodes * %d Bytes per Node = %.2f MB \n",totalNodes, sizeof(KdNode), (float)(totalNodes*sizeof(KdNode))/(float)(1024*1024));
}