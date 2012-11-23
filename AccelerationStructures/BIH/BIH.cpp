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

#include "BIH.h"

#ifdef LINUX
#include <sys/time.h>
#endif

// BVH Method Definitions
BIH::BIH(Primitive** a_Primitives, unsigned int a_NumPrimitives, uint32_t mp) 
{
	m_sName = "BIH";
    maxPrimsInNode = min(255u, mp);

	m_pPrimitives = a_Primitives;
	m_uiNumPrimitives = a_NumPrimitives;
}

void BIH::Build()
{
	if (m_uiNumPrimitives == 0) 
	{
        //nodes = NULL;
        return;
    }

	primitives.reserve(m_uiNumPrimitives);
	BBox fullBox;

	for (unsigned int i = 0; i < m_uiNumPrimitives; ++i) 
	{ 
		Point Vertex1(m_pPrimitives[i]->GetVertex(0)->Pos);
		Point Vertex2(m_pPrimitives[i]->GetVertex(1)->Pos);
		Point Vertex3(m_pPrimitives[i]->GetVertex(2)->Pos);
		BBox bbox = Union(BBox(Vertex1, Vertex2),Vertex3);

		primitives.push_back(BIHPrimitiveInfo(i, bbox));
		fullBox = Union(fullBox,bbox);
    }

	MemoryArena buildArena;
	totalNodes = 0;
	BIHNode* root = InitNode(buildArena, fullBox, 0, m_uiNumPrimitives-1, 0, 0, &totalNodes);

	nodes = AllocAligned<LinearBIHNode>(totalNodes);
    for (uint32_t i = 0; i < totalNodes; ++i)
	{
        new (&nodes[i]) LinearBIHNode;
	}
	uint32_t offset = 0;
    FlattenBIHTree(root, &offset);
}

BIHNode* BIH::InitNode(MemoryArena &buildArena, const BBox& bbox, int left, int right, int depth, int singleDepth, uint32_t *totalNodes)
{
	(*totalNodes)++;

	BIHNode* node = buildArena.Alloc<BIHNode>();
	
	if ((right - left + 1) <= maxPrimsInNode || depth > 100 || singleDepth > 12)
	{
		// init leaf
		node->InitLeaf(left,right);
	}
	else
	{
		uint32_t axis = bbox.MaximumExtent();
		SplitBIH split = SplitBIH((bbox.pMin[axis]+bbox.pMax[axis])/2,axis);

		int i = left, j = right;
		float minL, maxL, minR, maxR, position, minB, maxB;
		minL = minR = INFINITY;
		maxR = maxL = -INFINITY;

		while(i <= j)
		{
			BBox box = primitives[i].bounds;
			position = primitives[i].centroid[split.axis];
			// Store the limits of the box in (minB,maxB)
			minB = box.pMin[split.axis];
			maxB = box.pMax[split.axis];
			
			if (position <= split.position)
			{
				// Update the left-side limits against the current primitive box
				minL = min(minB, minL);
                maxL = max(maxB, maxL);
                i++;
			}
			else
			{
				// Update the right-side limits against the current primitive box
				minR = min(minB, minR);
                maxR = max(maxB, maxR);
				if(i != j)
				{
					swap(primitives[i],primitives[j]);
				}
				j--;
			}
		}

		// By this time, (minL,maxL) contains the limits that enclose all the primitives on the left side, (minR,maxR) do the same for the
		// right-side primitives

		Point minLP, maxLP, minRP, maxRP;
		minLP = minRP = bbox.pMin;
		maxLP = maxRP = bbox.pMax;
		maxLP[split.axis] = minRP[split.axis] = split.position;
		//(minLP-maxLP) is now a box that goes from the left side to the split plane)
		//(minRP-maxRP) is now a box that goes from the split plane to the right side)

		node->splitAxis = split;
		node->left = left;
		node->right = right;

		if(i > right) // If this happens then all primitives were sent to the left side
		//if(left < j)
		{
			BBox newBox(minLP,maxLP);
			node->clip[0] = minL;
			node->clip[1] = maxL; // Clipping planes for the left side
			node->children[0] = InitNode(buildArena, BBox(minLP,maxLP), left, right, depth+1, singleDepth + 1, totalNodes);
			node->children[1] = 0;
		}
		else if (j < left) // If this happens, all primitives were sent to the right side
		//else if (i < right) // If this happens, all primitives were sent to the right side
		{
			node->clip[0] = minR;
			node->clip[1] = maxR;
			node->children[0] = InitNode(buildArena, BBox(minRP,maxRP), left, right, depth+1, singleDepth + 1, totalNodes);
			node->children[1] = 0;
		}
		else
		{
			node->clip[0] = maxL;
			node->clip[1] = minR;
			node->children[0] = InitNode(buildArena, BBox(minLP,maxLP), left, j, depth+1, 0, totalNodes);
			node->children[1] = InitNode(buildArena, BBox(minRP,maxRP), i, right, depth+1, 0, totalNodes);
		}
	}

	return node;
}

uint32_t BIH::FlattenBIHTree(BIHNode *node, uint32_t *offset) 
{
    LinearBIHNode *linearNode = &nodes[*offset];
    uint32_t myOffset = (*offset)++;
    if (node->isLeaf())
	{
		Assert(!node->children[0] && !node->children[1]);
        linearNode->nPrimitives = node->right - node->left + 1;
		linearNode->primitivesOffset = (node->left << 3) | 0x03;
    }
    else 
	{
		linearNode->mClip[0] = node->clip[0];
		linearNode->mClip[1] = node->clip[1];
		
		if(node->children[0] != 0)
		{
			linearNode->secondChildOffset = node->splitAxis.axis;
			FlattenBIHTree(node->children[0], offset);
		}
		if(node->children[1] != 0)
		{
			linearNode->secondChildOffset = (FlattenBIHTree(node->children[1],offset) << 3) | node->splitAxis.axis;
		}
    }
	
	return myOffset;
}

/*
TIntersection BIH::IntersectP(VECTOR3 &Start,VECTOR3 &Direction,float maxT,int &candidates,int *MailBox,int MailBoxId)
{
	Point StartP(Start.x,Start.y,Start.z);
	Vector DirectionP(Direction.x,Direction.y,Direction.z);
	Ray ray(StartP,DirectionP,0.0f);

	ray.mint = 0.0f;
	ray.maxt = INFINITY;
	TIntersection intersect = traverse(root, ray, 0, INFINITY);

	return intersect;
}
*/

TIntersection BIH::IntersectP(Ray &a_Ray)
{
	TIntersection hit(-1, a_Ray.maxt);	
    if (!nodes) return hit;	

	float tMinS[64], tMaxS[64];
    uint32_t todo[64];
    
    uint32_t todoOffset = 0, nodeNum = 0;
	TIntersection test;	
	float tMin = 0.0f, tMax = INFINITY;
	tMinS[0] = 0.0f;
	tMaxS[0] = INFINITY;
	
	float t[2];
	
	LinearBIHNode *node;
	do
	{
		node = &nodes[nodeNum];

		if (tMin < hit.t) // Skip intervals which will not provide a closer hit
		{
			//tMax = min(tMax, hit.t);
			// Is this a leaf?
			if (node->isLeaf())
			{
				// Test each primitive in the leaf
				for(uint32_t i = 0; i < node->nPrimitives; i++)
				{
					TIntersection test(-1,INFINITY);
					int current = primitives[node->getPrimitivesOffset() + i].primitiveNumber;
					if (RayTriangleTest(a_Ray.o, a_Ray.d, test, current, m_pPrimitives) && test.t < hit.t)
					{
							test.IDTr = current;
							hit = test;
					}
					//candidates++;
				} // for
			} // leaf case
			else
			{
				uint32_t axis		= node->getSplitAxis(); // The name of the field is not explicit; but this is the axis for inner nodes
				float fInvDir		= 1.0f / a_Ray.d[axis];
				uint32_t iNear		= (a_Ray.d[axis] < 0.0f); // If the direction is positive, it will go from t[0] to t[1], hence if it's negative, it will go from t[1] to t[0]
				uint32_t iFar		= 1 - iNear;
				// t = (X              - fOrigin) / fDirection
				t[0] = (node->mClip[0] - a_Ray.o[axis]) * fInvDir;
				t[1] = (node->mClip[1] - a_Ray.o[axis]) * fInvDir;

				if(node->getSecondChildOffset() == 0) // Single left node
				{
					if ((t[iNear] > tMax || t[iFar] < tMin)){}
					else
					{
						todo[todoOffset] = nodeNum + 1;
						tMinS[todoOffset] = max(t[iNear], tMin);
						tMaxS[todoOffset] = min(t[iFar],  tMax);
						todoOffset++;
					}
				}
				else if (t[iNear] >= tMin || t[iFar] <= tMax)
				{
				// Inner node with exactly two children...
					
					// if tiNear < tMin then the left node cannot be searched, look on the right node only
					if(t[iNear] < tMin)
					{
						todo[todoOffset] = iFar ? node->getSecondChildOffset() : nodeNum + 1;
						tMinS[todoOffset] = max(tMin,t[iFar]);
						tMaxS[todoOffset] = tMax;
						
					}
					else if(t[iFar] > tMax)				
					{
					// if tiFar > tMax, then the left node cannot be traversed, look on the left node only
						todo[todoOffset] = iNear ? node->getSecondChildOffset() : nodeNum + 1;
						tMinS[todoOffset] = tMin;
						tMaxS[todoOffset] = min(tMax, hit.t);
					}				
					else
					{
						todo[todoOffset] = iFar ? node->getSecondChildOffset() : nodeNum + 1;
						tMinS[todoOffset] = max(tMin,t[iFar]);
						tMaxS[todoOffset] = tMax;
						todoOffset++;

						// Order is reversed because we're using a stack
						todo[todoOffset] = iNear ? node->getSecondChildOffset() : nodeNum + 1;
						tMinS[todoOffset] = tMin;
						tMaxS[todoOffset] = t[iNear];
					}
					todoOffset++;
				}
			} // Not a leaf case
		} // if (tMin < hit.t)

		// Failsafe checks
		if (todoOffset == 0) break;

		// Update the node and the t limits for the next iteration
		todoOffset--;
		nodeNum = todo[todoOffset];
		tMin	= tMinS[todoOffset];
		tMax	= tMaxS[todoOffset];
		//tMax	= min(hit.t, tMaxS[todoOffset]);
		//tMax = hit.t;
     } while (todoOffset >= 0);
	 
	 return hit;
}

void BIH::PrintOutput(float &tiempototal)
{
	printf("BIH: %d Nodes * %d Bytes per Node = %.2f MB \n",totalNodes, sizeof(LinearBIHNode), (float)(totalNodes*sizeof(LinearBIHNode))/(float)(1024*1024));
}

/*
uint32_t BIH::flattenBIHTree(BIHNode *node, uint32_t *offset) 
{
    LinearBIHNode *linearNode = &nodes[*offset];
    uint32_t myOffset = (*offset)++;
	
    if (node->isLeaf()) 
	{
		Assert(!node->children[0] && !node->children[1]);
        linearNode->iItems = 1;
		linearNode->iIndex |= 3;
    }
    else 
	{
		linearNode->mClip[0] = node->clip[0];
		linearNode->mClip[1] = node->clip[1];
		if(node->children[0] != 0)
		{
			linearNode->iIndex = node->splitAxis.axis;
			flattenBIHTree(node->children[0], offset);
		}
		if(node->children[1] != 0)
		{
			linearNode->iIndex = flattenBIHTree(node->children[1],offset) | node->splitAxis.axis;
		}
    }
	
	return myOffset;
}*/

BIH::~BIH() 
{
	FreeAligned(nodes);
}