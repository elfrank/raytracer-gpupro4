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

#include "LBVH.h"

#ifdef LINUX
#include <sys/time.h>
#endif

int cmpMorton(const void *_A, const void *_B)
{
	MortonCode *A=(MortonCode*)_A;
	MortonCode *B=(MortonCode*)_B;
	if (A->m_iCode < B->m_iCode)
	{
		return -1;
	}
	if (A->m_iCode > B->m_iCode)
	{
		return 1;
	}
	if (A->m_iId < B->m_iId)
	{
		return -1;
	}
	return 1;
}

inline Vector3 maxBox(Vector3 &A,Vector3 &B)
{
	Vector3 res;
	res.x=max(A.x,B.x);
	res.y=max(A.y,B.y);
	res.z=max(A.z,B.z);
	return res;
}

inline Vector3 minBox(Vector3 &A,Vector3 &B)
{
	Vector3 res;
	res.x=min(A.x,B.x);
	res.y=min(A.y,B.y);
	res.z=min(A.z,B.z);
	return res;
}

int bsf( const int x );
int bsr( const int x );

void radixsort(MortonCode *Morton,int count,int bit)
{
	if (bit==0)
		return;
	if (count<=1)
		return;
	int cero=0;
	while (cero<count)
	{
		if ((Morton[cero].m_iCode&bit)!=0)
		{
			break;
		}
		cero++;
	}
	if (cero==count)
	{
		//everything is 0
		radixsort(Morton,count,bit>>1);
		return;
	}
	int one=count-1;
	while (cero<one)
	{
		//find first non cero
		while (((Morton[cero].m_iCode&bit)==0)&&(cero!=one))
		{
			cero++;
		}
		//find last non 1
		while (((Morton[one].m_iCode&bit)!=0)&&(cero!=one))
		{
			one--;
		}
		//exchange
		if (cero!=one)
		{
			MortonCode aux = Morton[cero];
			Morton[cero]=Morton[one];
			Morton[one]=aux;
		}
	}
	radixsort(Morton,one,bit>>1);
	radixsort(Morton+one,count-one,bit>>1);
}

void LBVH::build_recursive_nlog2n(MortonCode *ordered_list, const unsigned int &NumPrim)
{
	/*
	radixsort(ordered_list,NumPrim,1<<30);
	
	// Create Tree
	m_pNodes[1].PrimCount = NumPrim;
	m_pNodes[1].PrimPos = 0;
	for (int i=2;i<m_iNumNodes;i+=2)
	{
		//check if the parent is a valid node
		int parent = i>>1;
		if (m_pNodes[parent].PrimCount <= 0)
		{
			//not a valid node
			m_pNodes[i].PrimPos = m_pNodes[i+1].PrimPos = -1;
			m_pNodes[i].PrimCount = m_pNodes[i+1].PrimCount = -1;
			continue;
		}
		//check if the parent is a leaf
		int a = m_pNodes[parent].PrimPos;
		int b = m_pNodes[parent].PrimPos+m_pNodes[parent].PrimCount-1;
		int mask = m_pPrimitivesMortonId[a].Code^m_pPrimitivesMortonId[b].Code;
		if (mask==0)
		{
			//it is a leaf, make inavlid both sons
			m_pNodes[i].PrimPos = m_pNodes[i+1].PrimPos = -1;
			m_pNodes[i].PrimCount = m_pNodes[i+1].PrimCount = -1;
			continue;
		}
		//it is an internal node, find the cut
		mask = 1<<bsr(mask);
		while ((b-a)>1)
		{
			int c = (b+a)/2;
			if ((m_pPrimitivesMortonId[c].Code&mask)==0)
			{
				a=c;
			}
			else
			{
				b=c;
			}
		}
		m_pNodes[i].PrimPos = m_pNodes[parent].PrimPos;
		m_pNodes[i].PrimCount = b-m_pNodes[i].PrimPos;
		m_pNodes[i+1].PrimPos = b;
		m_pNodes[i+1].PrimCount = m_pNodes[parent].PrimCount - m_pNodes[i].PrimCount;
		m_pNodes[parent].PrimCount = 0; //it is an internal node
	}
	
	//update the bounding boxes
	m_pNodes[1].PrimCount=0;
	for (int i = m_iNumNodes-1; i>0;i--)
	{
		//if it is inavlid, skip
		if (m_pNodes[i].PrimPos<0)
		{
			continue;
		}
		//if it is a leaf, take the bounding box of the primitives
		if (m_pNodes[i].PrimCount>0)
		{
			TMortonID *pPos=m_pPrimitivesMortonId+m_pNodes[i].PrimPos;
			m_pNodes[i].maxCoord = gMaxVertices[pPos[0].ID];
			m_pNodes[i].minCoord = gMinVertices[pPos[0].ID];
			for (int j=1;j<m_pNodes[i].PrimCount;j++)
			{
				m_pNodes[i].maxCoord = maxBox(m_pNodes[i].maxCoord,gMaxVertices[pPos[j].ID]);
				m_pNodes[i].minCoord = minBox(m_pNodes[i].minCoord,gMinVertices[pPos[j].ID]);
			}
		}
		//if it is an internal node, take the union of the bounding boxes
		else
		{
			m_pNodes[i].maxCoord = maxBox(m_pNodes[i<<1].maxCoord,m_pNodes[(i<<1)|1].maxCoord);
			m_pNodes[i].minCoord = minBox(m_pNodes[i<<1].minCoord,m_pNodes[(i<<1)|1].minCoord);
		}
	}
	*/
}

//void LBVH::build_recursive(int position,int count,unsigned __int64 bit,int node, int axis)
void LBVH::build_recursive(int position,int count,unsigned int bit,int node, int axis)
{
	MortonCode *Morton=m_pPrimitivesMortonId+position;

	if ( (bit == 0) || (count <= 1) || ( (node << 1) >= m_iNumNodes) )
	{
		//add to this node, every primitive has the same code
		++OccupiedNodes; 
		m_pNodes[node].PrimCount = (count << 3) | (4) | (axis);
		m_pNodes[node].PrimPos=position;
		m_pNodes[node].maxCoord = gMaxVertices[Morton[0].m_iId];
		m_pNodes[node].minCoord = gMinVertices[Morton[0].m_iId];
		for (int j = 1; j < count; ++j)
		{
			m_pNodes[node].maxCoord = maxBox(m_pNodes[node].maxCoord,gMaxVertices[Morton[j].m_iId]);
			m_pNodes[node].minCoord = minBox(m_pNodes[node].minCoord,gMinVertices[Morton[j].m_iId]);
		}
		++occupiedLeafs;
		return;
	}

	int cero = 0;
	while (cero < count)
	{
		if ((Morton[cero].m_iCode & bit) != 0)
		{
			break;
		}
		++cero;
	}

	/*int longest_axis = 0;
	float xDiff = abs( m_pNodes[node].maxCoord.x - m_pNodes[node].minCoord.x );
	float yDiff = abs( m_pNodes[node].maxCoord.y - m_pNodes[node].minCoord.y );
	float zDiff = abs( m_pNodes[node].maxCoord.z - m_pNodes[node].minCoord.z );

	if(yDiff > xDiff)
		longest_axis = 1;
	if(zDiff > yDiff)
		longest_axis = 2;*/

	if (cero == count)
	{
		//everything is 0
		build_recursive(position, count, bit>>1, node, (axis+1)%3);
		//build_recursive(position, count, bit>>1, node, longest_axis);
		return;
	}

	int one=count-1;
	while (cero<one)
	{
		//find first non cero
		while (((Morton[cero].m_iCode&bit)==0) && (cero!=one))
		{
			++cero;
		}
		//find last non 1
		while (((Morton[one].m_iCode&bit)!=0)&&(cero!=one))
		{
			--one;
		}
		//exchange
		if (cero != one)
		{
			MortonCode aux = Morton[cero];
			Morton[cero] = Morton[one];
			Morton[one] = aux;
		}
	}
	if ((Morton[0].m_iCode&bit)!=0)
	{
		//everything is 1
		build_recursive(position,count,bit>>1,node, (axis+1)%3);
		//build_recursive(position,count,bit>>1,node, longest_axis);
		return;
	}
	++OccupiedNodes;

	/*vector<TMortonID> right(count-one);
	vector<TMortonID> left(one);
	for(int i = 0; i < right.size(); ++i)
	{
		right[i] = Morton[one+i];
	}

	for(int i = 0; i < left.size(); ++i)
	{
		left[i] = Morton[i];
	}

	sort( right.begin(),right.end() );
	sort( left.begin(),left.end() );

	for(int i = 0; i < right.size(); ++i)
	{
		Morton[one+i] = right[i];
	}

	for(int i = 0; i < left.size(); ++i)
	{
		Morton[i] = left[i];
	}

	build_recursive(position,count/2,bit>>1,node<<1, (axis+1)%3);
	build_recursive(position+count/2,(count/2)+(count&1),bit>>1,(node<<1)|1, (axis+1)%3);*/

	/*float rate = static_cast<float>(one)/static_cast<float>(count);
	float k = 0.3f;
	if( rate < k || rate > (1.f-k) )
	{
		printf("%f\n",rate);
	}*/

	build_recursive(position,one,bit>>1,node<<1, (axis+1)%3);
	build_recursive(position+one,count-one,bit>>1,(node<<1)|1, (axis+1)%3);
	//build_recursive(position,one,bit>>1,node<<1, longest_axis);
	//build_recursive(position+one,count-one,bit>>1,(node<<1)|1, longest_axis);
	//make an internal node
	//m_pNodes[node].PrimCount = -count;
	m_pNodes[node].PrimCount = (count << 3) | (axis);
	m_pNodes[node].PrimPos = position;
	m_pNodes[node].maxCoord = maxBox(m_pNodes[node<<1].maxCoord,m_pNodes[(node<<1)|1].maxCoord);
	m_pNodes[node].minCoord = minBox(m_pNodes[node<<1].minCoord,m_pNodes[(node<<1)|1].minCoord);
}

void LBVH::build_sah(int position,int count,unsigned __int64 bit,int node, int axis)
{
	BBox bbox;

	for(int i = 0; i < count; ++i)
	{
		Union(bbox,m_pPrimitives[0]->GetVertex(0)->Pos);
	}

	float invSAH = 1/bbox.SurfaceArea();
	float cost[8];
    for (int i = 0; i < 8; ++i) 
	{
        BBox b0, b1;
        int count0 = 0, count1 = 0;
        for (int j = 0; j <= i; ++j) 
		{
			BBox b(m_pNodes[node].maxCoord , m_pNodes[node].minCoord);
            b0 = Union(b0, b);
            count0 += 1;
        }
        for (int j = i+1; j < 8; ++j) 
		{
			BBox b(m_pNodes[node].maxCoord , m_pNodes[node].minCoord);
            b1 = Union(b1, b);
            count1 += 1;
        }

        cost[i] = .125f + (count0 * b0.SurfaceArea() + count1 * b1.SurfaceArea()) * invSAH;
    }

	float minCost = cost[0];
    int minCostSplit = 0;
    for (int i = 1; i < 8; ++i) 
	{
        if (cost[i] < minCost) 
		{
            minCost = cost[i];
            minCostSplit = i;
        }
    }
}

void LBVH::build_iterative(MortonCode *ordered_list, const unsigned int &NUM_BITS, const unsigned int &SIZE)
{
	/*
	unsigned int i, bit, offset, count;
	TMortonID *list, current;
	list = new TMortonID[SIZE];
	
	// Radix sort for Morton Codes
	for(bit = 0; bit < NUM_BITS; bit++)
	{
		count = offset = 0;
		for(i = 0; i < SIZE; i++)
		{	
			current = ordered_list[i];	
			if ((current.Code>>bit)&1)
			{
				// If current bit is on, add to a tmp list
 				list[count++] = current;
			}
			else
			{
				// Copy values directly to the ordered array
				ordered_list[offset++] = current;
			}
		}
		// Copy final values of bit '1' to the ordered array
		memcpy(ordered_list+offset,list,sizeof(list[0])*count);
	}
	
	// Create Tree
	m_pNodes[1].PrimCount = SIZE;
	m_pNodes[1].PrimPos = 0;
	for (int i=2;i<m_iNumNodes;i+=2)
	{
		//check if the parent is a valid node
		int parent = i>>1;
		if (m_pNodes[parent].PrimCount <= 0)
		{
			//not a valid node
			m_pNodes[i].PrimPos = m_pNodes[i+1].PrimPos = -1;
			m_pNodes[i].PrimCount = m_pNodes[i+1].PrimCount = -1;
			continue;
		}
		//check if the parent is a leaf
		int a = m_pNodes[parent].PrimPos;
		int b = m_pNodes[parent].PrimPos+m_pNodes[parent].PrimCount-1;
		int mask = m_pPrimitivesMortonId[a].Code^m_pPrimitivesMortonId[b].Code;
		if (mask==0)
		{
			//it is a leaf, make inavlid both sons
			m_pNodes[i].PrimPos = m_pNodes[i+1].PrimPos = -1;
			m_pNodes[i].PrimCount = m_pNodes[i+1].PrimCount = -1;
			continue;
		}
		//it is an internal node, find the cut
		mask = 1<<bsr(mask);
		while ((b-a)>1)
		{
			int c = (b+a)/2;
			if ((m_pPrimitivesMortonId[c].Code&mask)==0)
			{
				a=c;
			}
			else
			{
				b=c;
			}
		}
		m_pNodes[i].PrimPos = m_pNodes[parent].PrimPos;
		m_pNodes[i].PrimCount = b-m_pNodes[i].PrimPos;
		m_pNodes[i+1].PrimPos = b;
		m_pNodes[i+1].PrimCount = m_pNodes[parent].PrimCount - m_pNodes[i].PrimCount;
		m_pNodes[parent].PrimCount = 0; //it is an internal node
	}
	
	//update the bounding boxes
	m_pNodes[1].PrimCount=0;
	for (int i = m_iNumNodes-1; i>0;i--)
	{
		//if it is inavlid, skip
		if (m_pNodes[i].PrimPos<0)
		{
			continue;
		}
		//if it is a leaf, take the bounding box of the primitives
		if (m_pNodes[i].PrimCount>0)
		{
			TMortonID *pPos=m_pPrimitivesMortonId+m_pNodes[i].PrimPos;
			m_pNodes[i].maxCoord = gMaxVertices[pPos[0].ID];
			m_pNodes[i].minCoord = gMinVertices[pPos[0].ID];
			for (int j=1;j<m_pNodes[i].PrimCount;j++)
			{
				m_pNodes[i].maxCoord = maxBox(m_pNodes[i].maxCoord,gMaxVertices[pPos[j].ID]);
				m_pNodes[i].minCoord = minBox(m_pNodes[i].minCoord,gMinVertices[pPos[j].ID]);
			}
		}
		//if it is an internal node, take the union of the bounding boxes
		else
		{
			m_pNodes[i].maxCoord = maxBox(m_pNodes[i<<1].maxCoord,m_pNodes[(i<<1)+1].maxCoord);
			m_pNodes[i].minCoord = minBox(m_pNodes[i<<1].minCoord,m_pNodes[(i<<1)+1].minCoord);
		}
	}
	
	delete[] list;
	*/
}

LBVH::LBVH(Primitive** pPrimitives, unsigned int uiNumPrimitives, int n)
{
	m_iInitNode=1;
	m_sName="LBVH";
	m_iNumNodes = n;
	m_pPrimitives = pPrimitives;
	m_uiNumPrimitives = uiNumPrimitives;
	gMinVertices = new Vector3[m_uiNumPrimitives*3];
	gMaxVertices = gMinVertices + m_uiNumPrimitives;
	gCentroid = gMaxVertices + m_uiNumPrimitives;

	OccupiedNodes = 0;
	occupiedLeafs = 0;
}

LBVH::~LBVH()
{
	delete[] m_pPrimitivesIds;
	delete[] m_pNodes;
}

void LBVH::Build()
{
	m_uiCandidates = 0;
	OccupiedNodes = 0;
//	Vector3 VertMax,VertMin;
	Vector3 VertMax,VertMin;
	//find max, min and centroids
	if (m_uiNumPrimitives > 0)
	{
		VertMax = VertMin = m_pPrimitives[0]->GetVertex(0)->Pos;
		
	}
	int Tid=0;
	for (unsigned int i = 0; i < m_uiNumPrimitives; i++)
	{
		Primitive* currentPrim = m_pPrimitives[i];
		//Point A(minBox(Vertices[Indices[Tid+1]].Pos,Vertices[Indices[Tid+2]].Pos));
		Point A(minBox(currentPrim->GetVertex(1)->Pos,currentPrim->GetVertex(2)->Pos));
		gMinVertices[i] = minBox(currentPrim->GetVertex(0)->Pos,A);

		Point B(maxBox(currentPrim->GetVertex(1)->Pos,currentPrim->GetVertex(2)->Pos));
		gMaxVertices[i] = maxBox(currentPrim->GetVertex(0)->Pos,B);

		//gCentroid[i] =  Vertices[Indices[Tid]].Pos + Vertices[Indices[Tid+1]].Pos + Vertices[Indices[Tid+2]].Pos;
		gCentroid[i] =  currentPrim->GetVertex(0)->Pos + currentPrim->GetVertex(1)->Pos + currentPrim->GetVertex(2)->Pos;
		VertMax = maxBox(VertMax,gMaxVertices[i]);
		VertMin = minBox(VertMin,gMinVertices[i]);
		Tid+=3;
	}

	printf("\n\nMax(%f,%f,%f),Min(%f,%f,%f)\n\n\n",VertMax.x,VertMax.y,VertMax.z,VertMin.x,VertMin.y,VertMin.z);

	//assign every primitive a Morton code
	VertMax = 3.0f*VertMax;
	VertMin = 3.0f*VertMin;
	Vector3 intfactor(VertMax.x-VertMin.x, VertMax.y-VertMin.y, VertMax.z-VertMin.z);
	intfactor.x = 1024.f / intfactor.x;
	intfactor.y = 1024.f / intfactor.y;
	intfactor.z = 1024.f / intfactor.z;
	/*intfactor.x = 2097152.0f/intfactor.x;
	intfactor.y = 2097152.0f/intfactor.y;
	intfactor.z = 2097152.0f/intfactor.z;*/
	m_pPrimitivesMortonId = new MortonCode[m_uiNumPrimitives];
	//unsigned __int64 shift3=0,shift=2;
	unsigned int shift3=0,shift=2;
	for (unsigned int i = 0; i < m_uiNumPrimitives; ++i)
	{
		/*unsigned __int64 inx = int((gCentroid[i].x-VertMin.x)*intfactor.x);
		unsigned __int64 iny = int((gCentroid[i].y-VertMin.y)*intfactor.y);
		unsigned __int64 inz = int((gCentroid[i].z-VertMin.z)*intfactor.z);*/
		unsigned int inx = int((gCentroid[i].x-VertMin.x)*intfactor.x);
		unsigned int iny = int((gCentroid[i].y-VertMin.y)*intfactor.y);
		unsigned int inz = int((gCentroid[i].z-VertMin.z)*intfactor.z);
		m_pPrimitivesMortonId[i].m_iCode=(inz&1)|((iny&1)<<1)|((inx&1)<<2);
		m_pPrimitivesMortonId[i].m_iId = i;
		//unsigned __int64 shift3=2,shift=2;
		unsigned int shift3=2,shift=2;
		//for (int j = 1; j < 21; j++)
		for (int j = 1; j < 10; j++)
		{
			m_pPrimitivesMortonId[i].m_iCode|=(inz&shift)<<(shift3++);
			m_pPrimitivesMortonId[i].m_iCode|=(iny&shift)<<(shift3++);
			m_pPrimitivesMortonId[i].m_iCode|=(inx&shift)<<(shift3);
			shift<<=1;
		}
	}

	/*printf("\n\n");
	printf("intfactor(%f,%f,%f)\n", intfactor.x,intfactor.y,intfactor.z);
	for(int i = 0; i < 10; i++)
	{
		printf("[%d]:%d\n", m_pPrimitivesMortonId[i].m_iId,m_pPrimitivesMortonId[i].m_iCode);
	}
	printf("\n\n");*/

	/*char aux[1024];
	sprintf_s(aux,sizeof(aux)/sizeof(aux[0]),"\nintfactor(%f,%f,%f)\n", intfactor.x,intfactor.y,intfactor.z);
	OutputDebugStringA(aux);
	sprintf_s(aux,sizeof(aux)/sizeof(aux[0]),"\nUnordered Prims:\n");
	OutputDebugStringA(aux);
	for(int i = 0; i < m_uiNumPrimitives; i++)
	{
		//printf("[%d]:%d\n", m_pPrimitivesMortonId[i].m_iId,m_pPrimitivesMortonId[i].m_iCode);
		sprintf_s(aux,sizeof(aux)/sizeof(aux[0]),"[%d]:%d\n", m_pPrimitivesMortonId[i].m_iId,m_pPrimitivesMortonId[i].m_iCode);
		OutputDebugStringA(aux);
	}*/
	//printf("\n\n");


	m_pNodes = new LBVHNode[m_iNumNodes];
	//m_pNodes = m_pNodes;
	m_iNumNodes = m_iNumNodes;
	
	//build_recursive(0,m_uiNumPrimitives,1LL<<62,1, 0);
	build_recursive(0,m_uiNumPrimitives,1<<30,1, 0);
	//build_recursive_nlog2n(m_pPrimitivesMortonId,NumPrim);
	//build_iterative(m_pPrimitivesMortonId,30,NumPrim);

	/*sprintf_s(aux,sizeof(aux)/sizeof(aux[0]),"\nOrdered Prims:\n");
	OutputDebugStringA(aux);
	for(int i = 0; i < m_uiNumPrimitives; i++)
	{
		//printf("[%d]:%d\n", m_pPrimitivesMortonId[i].m_iId,m_pPrimitivesMortonId[i].m_iCode);
		sprintf_s(aux,sizeof(aux)/sizeof(aux[0]),"[%d]:%d\n", m_pPrimitivesMortonId[i].m_iId,m_pPrimitivesMortonId[i].m_iCode);
		OutputDebugStringA(aux);
	}

	sprintf_s(aux,sizeof(aux)/sizeof(aux[0]),"\nNodes:\n");
	OutputDebugStringA(aux);
	for (unsigned int i = 0; i < m_iNumNodes; ++i)
	{
		sprintf_s(aux,sizeof(aux)/sizeof(aux[0]),"[%d]: PrimPos=%d | PrimCount=%d | axis=%d | isleaf=%d\n", i, m_pNodes[i].PrimPos, (m_pNodes[i].PrimCount>>3), (m_pNodes[i].PrimCount&3), (m_pNodes[i].PrimCount&4)>>2);
		OutputDebugStringA(aux);
	}

	sprintf_s(aux,sizeof(aux)/sizeof(aux[0]),"\nNode Box:\n");
	OutputDebugStringA(aux);
	for (unsigned int i = 0; i < m_iNumNodes; ++i)
	{
		sprintf_s(aux,sizeof(aux)/sizeof(aux[0]),"[%d]: Min(%f,%f,%f)| Max(%f,%f,%f)\n", i, m_pNodes[i].minCoord.x,m_pNodes[i].minCoord.y,m_pNodes[i].minCoord.z, m_pNodes[i].maxCoord.x,m_pNodes[i].maxCoord.y,m_pNodes[i].maxCoord.z);
		OutputDebugStringA(aux);
	}*/

	/*char aux[1024];
	for(unsigned int i = 0; i < m_uiNumPrimitives; ++i)
	{
		printf("%d: id=%d, code=%d\n",i,m_pPrimitivesMortonId[i].ID,m_pPrimitivesMortonId[i].Code);
	}*/
	/*for (unsigned int i = 0; i < 10; ++i)
	{
		printf("{%d} ", i);
		m_pNodes[i].PrintString();
		//printf("\n\nMax(%f,%f,%f),Min(%f,%f,%f)\n\n\n",VertMax.x,VertMax.y,VertMax.z,VertMin.x,VertMin.y,VertMin.z);
	}*/
	
	
	//Copy m_pPrimitivesIds without ID's
	m_pPrimitivesIds = new int[m_uiNumPrimitives];
	for (unsigned int i = 0; i < m_uiNumPrimitives; ++i)
	{
		m_pPrimitivesIds[i] = m_pPrimitivesMortonId[i].m_iId;
	}
	delete[] m_pPrimitivesMortonId;
	delete[] gMinVertices;
}

inline void RayBoxInterval(Point &Start,Vector &DirectionOver,LBVHNode &Node,float &T0,float &T1)
{
	Vector3 DiffMax(Node.maxCoord - Start);
	DiffMax.x*=DirectionOver.x;
	DiffMax.y*=DirectionOver.y;
	DiffMax.z*=DirectionOver.z;
	Vector3 DiffMin(Node.minCoord-Start);
	DiffMin.x*=DirectionOver.x;
	DiffMin.y*=DirectionOver.y;
	DiffMin.z*=DirectionOver.z;

	T0=min(DiffMin.x,DiffMax.x);
	T1=max(DiffMin.x,DiffMax.x);

	T0=max(T0,min(DiffMin.y,DiffMax.y));
	T1=min(T1,max(DiffMin.y,DiffMax.y));

	T0=max(T0,min(DiffMin.z,DiffMax.z));
	T1=min(T1,max(DiffMin.z,DiffMax.z));

	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
	}
}

inline void RayBoxIntervalppp(Vector3 &Start,Vector3 &DirectionOver,LBVHNode &Node,float &T0,float &T1)
{
	Vector3 DiffMax(Node.maxCoord - Start);
	DiffMax.x*=DirectionOver.x;
	DiffMax.y*=DirectionOver.y;
	DiffMax.z*=DirectionOver.z;
	Vector3 DiffMin(Node.minCoord-Start);
	DiffMin.x*=DirectionOver.x;
	DiffMin.y*=DirectionOver.y;
	DiffMin.z*=DirectionOver.z;

	T0=DiffMin.x;
	T1=DiffMax.x;
	T0=max(T0,DiffMin.y);
	T1=min(T1,DiffMax.y);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
		return;
	}
	T0=max(T0,DiffMin.z);
	T1=min(T1,DiffMax.z);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
	}
}

inline void RayBoxIntervalppn(Vector3 &Start,Vector3 &DirectionOver,LBVHNode &Node,float &T0,float &T1)
{
	Vector3 DiffMax = Node.maxCoord-Start;
	DiffMax.x*=DirectionOver.x;
	DiffMax.y*=DirectionOver.y;
	DiffMax.z*=DirectionOver.z;
	Vector3 DiffMin = Node.minCoord-Start;
	DiffMin.x*=DirectionOver.x;
	DiffMin.y*=DirectionOver.y;
	DiffMin.z*=DirectionOver.z;

	T0=DiffMin.x;
	T1=DiffMax.x;
	T0=max(T0,DiffMin.y);
	T1=min(T1,DiffMax.y);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
		return;
	}
	T0=max(T0,DiffMax.z);
	T1=min(T1,DiffMin.z);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
	}
}

inline void RayBoxIntervalpnp(Vector3 &Start,Vector3 &DirectionOver,LBVHNode &Node,float &T0,float &T1)
{
	Vector3 DiffMax = Node.maxCoord-Start;
	DiffMax.x*=DirectionOver.x;
	DiffMax.y*=DirectionOver.y;
	DiffMax.z*=DirectionOver.z;
	Vector3 DiffMin = Node.minCoord-Start;
	DiffMin.x*=DirectionOver.x;
	DiffMin.y*=DirectionOver.y;
	DiffMin.z*=DirectionOver.z;

	T0=DiffMin.x;
	T1=DiffMax.x;
	T0=max(T0,DiffMax.y);
	T1=min(T1,DiffMin.y);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
		return;
	}
	T0=max(T0,DiffMin.z);
	T1=min(T1,DiffMax.z);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
	}
}

inline void RayBoxIntervalpnn(Vector3 &Start,Vector3 &DirectionOver,LBVHNode &Node,float &T0,float &T1)
{
	Vector3 DiffMax = Node.maxCoord-Start;
	DiffMax.x*=DirectionOver.x;
	DiffMax.y*=DirectionOver.y;
	DiffMax.z*=DirectionOver.z;
	Vector3 DiffMin = Node.minCoord-Start;
	DiffMin.x*=DirectionOver.x;
	DiffMin.y*=DirectionOver.y;
	DiffMin.z*=DirectionOver.z;

	T0=DiffMin.x;
	T1=DiffMax.x;
	T0=max(T0,DiffMax.y);
	T1=min(T1,DiffMin.y);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
		return;
	}
	T0=max(T0,DiffMax.z);
	T1=min(T1,DiffMin.z);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
	}
}

inline void RayBoxIntervalnpp(Vector3 &Start,Vector3 &DirectionOver,LBVHNode &Node,float &T0,float &T1)
{
	Vector3 DiffMax = Node.maxCoord-Start;
	DiffMax.x*=DirectionOver.x;
	DiffMax.y*=DirectionOver.y;
	DiffMax.z*=DirectionOver.z;
	Vector3 DiffMin = Node.minCoord-Start;
	DiffMin.x*=DirectionOver.x;
	DiffMin.y*=DirectionOver.y;
	DiffMin.z*=DirectionOver.z;

	T0=DiffMax.x;
	T1=DiffMin.x;
	T0=max(T0,DiffMin.y);
	T1=min(T1,DiffMax.y);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
		return;
	}
	T0=max(T0,DiffMin.z);
	T1=min(T1,DiffMax.z);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
	}
}

inline void RayBoxIntervalnpn(Vector3 &Start,Vector3 &DirectionOver,LBVHNode &Node,float &T0,float &T1)
{
	Vector3 DiffMax = Node.maxCoord-Start;
	DiffMax.x*=DirectionOver.x;
	DiffMax.y*=DirectionOver.y;
	DiffMax.z*=DirectionOver.z;
	Vector3 DiffMin = Node.minCoord-Start;
	DiffMin.x*=DirectionOver.x;
	DiffMin.y*=DirectionOver.y;
	DiffMin.z*=DirectionOver.z;

	T0=DiffMax.x;
	T1=DiffMin.x;
	T0=max(T0,DiffMin.y);
	T1=min(T1,DiffMax.y);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
		return;
	}
	T0=max(T0,DiffMax.z);
	T1=min(T1,DiffMin.z);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
	}
}

inline void RayBoxIntervalnnp(Vector3 &Start,Vector3 &DirectionOver,LBVHNode &Node,float &T0,float &T1)
{
	Vector3 DiffMax = Node.maxCoord-Start;
	DiffMax.x*=DirectionOver.x;
	DiffMax.y*=DirectionOver.y;
	DiffMax.z*=DirectionOver.z;
	Vector3 DiffMin = Node.minCoord-Start;
	DiffMin.x*=DirectionOver.x;
	DiffMin.y*=DirectionOver.y;
	DiffMin.z*=DirectionOver.z;

	T0=DiffMax.x;
	T1=DiffMin.x;
	T0=max(T0,DiffMax.y);
	T1=min(T1,DiffMin.y);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
		return;
	}
	T0=max(T0,DiffMin.z);
	T1=min(T1,DiffMax.z);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
	}
}

inline void RayBoxIntervalnnn(Vector3 &Start,Vector3 &DirectionOver,LBVHNode &Node,float &T0,float &T1)
{
	Vector3 DiffMax = Node.maxCoord-Start;
	DiffMax.x*=DirectionOver.x;
	DiffMax.y*=DirectionOver.y;
	DiffMax.z*=DirectionOver.z;
	Vector3 DiffMin = Node.minCoord-Start;
	DiffMin.x*=DirectionOver.x;
	DiffMin.y*=DirectionOver.y;
	DiffMin.z*=DirectionOver.z;

	T0=DiffMax.x;
	T1=DiffMin.x;
	T0=max(T0,DiffMax.y);
	T1=min(T1,DiffMin.y);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
		return;
	}
	T0=max(T0,DiffMax.z);
	T1=min(T1,DiffMin.z);
	if (T0>T1)
	{
		//empty interval
		T0=T1=-1.0f;
	}
}


/*inline int nextnode(const int &node)
{
	//int p = 0;
	int x = ((((node+1)^node)+1)>>1);
	/*if (x&0xAAAAAAAA)p+=1;
	if (x&0xCCCCCCCC)p+=2;
	if (x&0xF0F0F0F0)p+=4;
	if (x&0xFF00FF00)p+=8;
	if (x&0xFFFF0000)p+=16;*/
/*	return (node>>bsf(x))+1;
}*/

inline int firstson(int node)
{
	return node<<1;
}

inline TIntersection LBVH::IntersectP(Ray &a_Ray)
{	
	/*
	if (a_Ray.d.x<0.0f)
	{
		if (a_Ray.d.y<0.0f)
		{
			if (a_Ray.d.z<0.0f)
			{
				return intersectPnnn(a_Ray);
			}
			else
			{
				return intersectPnnp(a_Ray);
			}
		}
		else
		{
			if (a_Ray.d.z<0.0f)
			{
				return intersectPnpn(a_Ray);
			}
			else
			{
				return intersectPnpp(a_Ray);
			}
		}
	}
	else
	{
		if (a_Ray.d.y<0.0f)
		{
			if (a_Ray.d.z<0.0f)
			{
				return intersectPpnn(a_Ray);
			}
			else
			{
				return intersectPpnp(a_Ray);
			}
		}
		else
		{
			if (a_Ray.d.z<0.0f)
			{
				return intersectPppn(a_Ray);
			}
			else
			{
				return intersectPppp(a_Ray);
			}
		}
	}*/
		
	TIntersection res(-1,a_Ray.maxt);
	TIntersection test;
	int node=1;
	int mask=1;
	float T0,T1;
	Vector DirectionOver;
	DirectionOver.x=1.0f/a_Ray.d.x;
	DirectionOver.y=1.0f/a_Ray.d.y;
	DirectionOver.z=1.0f/a_Ray.d.z;
	int signo[3];
	signo[0]=a_Ray.d.x<0.0f?1:0;
	signo[1]=a_Ray.d.y<0.0f?1:0;
	signo[2]=a_Ray.d.z<0.0f?1:0;

	do
	{
		//see if this node intersects with the ray
		RayBoxInterval(a_Ray.o,DirectionOver,m_pNodes[node],T0,T1);
		//m_Candidates++;
		if ((T1<0.0f)||(T0>res.t))
		{
			//does not intersect, change to next node
			++mask;
			int p = bsf(mask);
			node=(node>>p)^1;
			mask=(mask>>p);
		}
		else
		{
			if ( (m_pNodes[node].PrimCount & 4 )!=0)
			{
				//it is a leaf, compare against all the primitives and asign next node			
				int *offset=m_pPrimitivesIds+m_pNodes[node].PrimPos;
				for (int i = (m_pNodes[node].PrimCount>>3)-1; i >= 0; i--)
				{
					int idTr = offset[i];
					if (a_Ray.TriangleID == idTr)
					{
						continue;
					}
					////m_Candidates++;
					// Check if there is a Triangle Intersection
					a_Ray.TriangleID = idTr;					
					if (RayTriangleTest(a_Ray.o,a_Ray.d,test,idTr,m_pPrimitives))
					{
						if ( test.t < res.t)
						{
							res = test;
							res.IDTr = idTr;
						}
					}
				}
				++mask;
				int p = bsf(mask);
				node=(node>>p)^1;
				mask=(mask>>p);
			}
			else
			{
				//not a leaf, go to first son
				node=node<<1 | signo[m_pNodes[node].PrimCount&3];
				mask=mask<<1;
			}
		}
	}while (mask!=1);
	return res;
}

inline TIntersection LBVH::intersectPppp(Ray &a_Ray)
{
	TIntersection res(-1,a_Ray.maxt);
	TIntersection test;
	int node=1;
	int mask=1;
	float T0,T1;
	Vector DirectionOver;
	DirectionOver.x=1.0f/a_Ray.d.x;
	DirectionOver.y=1.0f/a_Ray.d.y;
	DirectionOver.z=1.0f/a_Ray.d.z;
	int signo[3]={0,0,0};

	do
	{
		//see if this node intersects with the ray
		RayBoxIntervalppp(a_Ray.o,DirectionOver,m_pNodes[node],T0,T1);
		//m_Candidates++;
		if ((T1<0.0f)||(T0>res.t))
		{
			//does not intersect, change to next node
			++mask;
			int p = bsf(mask);
			node=(node>>p)^1;
			mask=(mask>>p);
		}
		else
		{
			if ( (m_pNodes[node].PrimCount & 4 )!=0)
			{
				//it is a leaf, compare against all the primitives and asign next node			
				int *offset=m_pPrimitivesIds+m_pNodes[node].PrimPos;
				for (int i = (m_pNodes[node].PrimCount>>3)-1; i >= 0; i--)
				{
					int idTr = offset[i];
					if (a_Ray.TriangleID == idTr)
					{
						continue;
					}
					////m_Candidates++;
					// Check if there is a Triangle Intersection
					a_Ray.TriangleID = idTr;					
					if (RayTriangleTest(a_Ray.o,a_Ray.d,test,idTr,m_pPrimitives))
					{
						if ( test.t < res.t)
						{
							res = test;
							res.IDTr = idTr;
						}
					}
				}
				++mask;
				int p = bsf(mask);
				node=(node>>p)^1;
				mask=(mask>>p);
			}
			else
			{
				//not a leaf, go to first son
				node=node<<1 | signo[m_pNodes[node].PrimCount&3];
				mask=mask<<1;
			}
		}
	}while (mask!=1);
	return res;
}

inline TIntersection LBVH::intersectPppn(Ray &a_Ray)
{
	TIntersection res(-1,a_Ray.maxt);
	TIntersection test;
	int node=1;
	int mask=1;
	float T0,T1;
	Vector DirectionOver;
	DirectionOver.x=1.0f/a_Ray.d.x;
	DirectionOver.y=1.0f/a_Ray.d.y;
	DirectionOver.z=1.0f/a_Ray.d.z;
	int signo[3]={0,0,1};

	do
	{
		//see if this node intersects with the ray
		RayBoxIntervalppn(a_Ray.o,DirectionOver,m_pNodes[node],T0,T1);
		//m_Candidates++;
		if ((T1<0.0f)||(T0>res.t))
		{
			//does not intersect, change to next node
			++mask;
			int p = bsf(mask);
			node=(node>>p)^1;
			mask=(mask>>p);
		}
		else
		{
			if ( (m_pNodes[node].PrimCount & 4 )!=0)
			{
				//it is a leaf, compare against all the primitives and asign next node			
				int *offset=m_pPrimitivesIds+m_pNodes[node].PrimPos;
				for (int i = (m_pNodes[node].PrimCount>>3)-1; i >= 0; i--)
				{
					int idTr = offset[i];
					if (a_Ray.TriangleID == idTr)
					{
						continue;
					}
					////m_Candidates++;
					// Check if there is a Triangle Intersection
					a_Ray.TriangleID = idTr;					
					if (RayTriangleTest(a_Ray.o,a_Ray.d,test,idTr,m_pPrimitives))
					{
						if ( test.t < res.t)
						{
							res = test;
							res.IDTr = idTr;
						}
					}
				}
				++mask;
				int p = bsf(mask);
				node=(node>>p)^1;
				mask=(mask>>p);
			}
			else
			{
				//not a leaf, go to first son
				node=node<<1 | signo[m_pNodes[node].PrimCount&3];
				mask=mask<<1;
			}
		}
	}while (mask!=1);
	return res;
}

inline TIntersection LBVH::intersectPpnp(Ray &a_Ray)
{
	TIntersection res(-1,a_Ray.maxt);
	TIntersection test;
	int node=1;
	int mask=1;
	float T0,T1;
	Vector DirectionOver;
	DirectionOver.x=1.0f/a_Ray.d.x;
	DirectionOver.y=1.0f/a_Ray.d.y;
	DirectionOver.z=1.0f/a_Ray.d.z;
	int signo[3]={0,1,0};

	do
	{
		//see if this node intersects with the ray
		RayBoxIntervalpnp(a_Ray.o,DirectionOver,m_pNodes[node],T0,T1);
		//m_Candidates++;
		if ((T1<0.0f)||(T0>res.t))
		{
			//does not intersect, change to next node
			++mask;
			int p = bsf(mask);
			node=(node>>p)^1;
			mask=(mask>>p);
		}
		else
		{
			if ( (m_pNodes[node].PrimCount & 4 )!=0)
			{
				//it is a leaf, compare against all the primitives and asign next node			
				int *offset=m_pPrimitivesIds+m_pNodes[node].PrimPos;
				for (int i = (m_pNodes[node].PrimCount>>3)-1; i >= 0; i--)
				{
					int idTr = offset[i];
					if (a_Ray.TriangleID == idTr)
					{
						continue;
					}
					////m_Candidates++;
					// Check if there is a Triangle Intersection
					a_Ray.TriangleID = idTr;					
					if (RayTriangleTest(a_Ray.o,a_Ray.d,test,idTr,m_pPrimitives))
					{
						if ( test.t < res.t)
						{
							res = test;
							res.IDTr = idTr;
						}
					}
				}
				++mask;
				int p = bsf(mask);
				node=(node>>p)^1;
				mask=(mask>>p);
			}
			else
			{
				//not a leaf, go to first son
				node=node<<1 | signo[m_pNodes[node].PrimCount&3];
				mask=mask<<1;
			}
		}
	}while (mask!=1);
	return res;
}

inline TIntersection LBVH::intersectPpnn(Ray &a_Ray)
{
	TIntersection res(-1,a_Ray.maxt);
	TIntersection test;
	int node=1;
	int mask=1;
	float T0,T1;
	Vector DirectionOver;
	DirectionOver.x=1.0f/a_Ray.d.x;
	DirectionOver.y=1.0f/a_Ray.d.y;
	DirectionOver.z=1.0f/a_Ray.d.z;
	int signo[3]={0,1,1};

	do
	{
		//see if this node intersects with the ray
		RayBoxIntervalpnn(a_Ray.o,DirectionOver,m_pNodes[node],T0,T1);
		//m_Candidates++;
		if ((T1<0.0f)||(T0>res.t))
		{
			//does not intersect, change to next node
			++mask;
			int p = bsf(mask);
			node=(node>>p)^1;
			mask=(mask>>p);
		}
		else
		{
			if ( (m_pNodes[node].PrimCount & 4 )!=0)
			{
				//it is a leaf, compare against all the primitives and asign next node			
				int *offset=m_pPrimitivesIds+m_pNodes[node].PrimPos;
				for (int i = (m_pNodes[node].PrimCount>>3)-1; i >= 0; i--)
				{
					int idTr = offset[i];
					if (a_Ray.TriangleID == idTr)
					{
						continue;
					}
					////m_Candidates++;
					// Check if there is a Triangle Intersection
					a_Ray.TriangleID = idTr;					
					if (RayTriangleTest(a_Ray.o,a_Ray.d,test,idTr,m_pPrimitives))
					{
						if ( test.t < res.t)
						{
							res = test;
							res.IDTr = idTr;
						}
					}
				}
				++mask;
				int p = bsf(mask);
				node=(node>>p)^1;
				mask=(mask>>p);
			}
			else
			{
				//not a leaf, go to first son
				node=node<<1 | signo[m_pNodes[node].PrimCount&3];
				mask=mask<<1;
			}
		}
	}while (mask!=1);
	return res;
}

inline TIntersection LBVH::intersectPnpp(Ray &a_Ray)
{
	TIntersection res(-1,a_Ray.maxt);
	TIntersection test;
	int node=1;
	int mask=1;
	float T0,T1;
	Vector DirectionOver;
	DirectionOver.x=1.0f/a_Ray.d.x;
	DirectionOver.y=1.0f/a_Ray.d.y;
	DirectionOver.z=1.0f/a_Ray.d.z;
	int signo[3]={1,0,0};

	do
	{
		//see if this node intersects with the ray
		RayBoxIntervalnpp(a_Ray.o,DirectionOver,m_pNodes[node],T0,T1);
		//m_Candidates++;
		if ((T1<0.0f)||(T0>res.t))
		{
			//does not intersect, change to next node
			++mask;
			int p = bsf(mask);
			node=(node>>p)^1;
			mask=(mask>>p);
		}
		else
		{
			if ( (m_pNodes[node].PrimCount & 4 )!=0)
			{
				//it is a leaf, compare against all the primitives and asign next node			
				int *offset=m_pPrimitivesIds+m_pNodes[node].PrimPos;
				for (int i = (m_pNodes[node].PrimCount>>3)-1; i >= 0; i--)
				{
					int idTr = offset[i];
					if (a_Ray.TriangleID == idTr)
					{
						continue;
					}
					////m_Candidates++;
					// Check if there is a Triangle Intersection
					a_Ray.TriangleID = idTr;					
					if (RayTriangleTest(a_Ray.o,a_Ray.d,test,idTr,m_pPrimitives))
					{
						if ( test.t < res.t)
						{
							res = test;
							res.IDTr = idTr;
						}
					}
				}
				++mask;
				int p = bsf(mask);
				node=(node>>p)^1;
				mask=(mask>>p);
			}
			else
			{
				//not a leaf, go to first son
				node=node<<1 | signo[m_pNodes[node].PrimCount&3];
				mask=mask<<1;
			}
		}
	}while (mask!=1);
	return res;
}

inline TIntersection LBVH::intersectPnpn(Ray &a_Ray)
{
	TIntersection res(-1,a_Ray.maxt);
	TIntersection test;
	int node=1;
	int mask=1;
	float T0,T1;
	Vector DirectionOver;
	DirectionOver.x=1.0f/a_Ray.d.x;
	DirectionOver.y=1.0f/a_Ray.d.y;
	DirectionOver.z=1.0f/a_Ray.d.z;
	int signo[3]={1,0,1};

	do
	{
		//see if this node intersects with the ray
		RayBoxIntervalnpn(a_Ray.o,DirectionOver,m_pNodes[node],T0,T1);
		//m_Candidates++;
		if ((T1<0.0f)||(T0>res.t))
		{
			//does not intersect, change to next node
			++mask;
			int p = bsf(mask);
			node=(node>>p)^1;
			mask=(mask>>p);
		}
		else
		{
			if ( (m_pNodes[node].PrimCount & 4 )!=0)
			{
				//it is a leaf, compare against all the primitives and asign next node			
				int *offset=m_pPrimitivesIds+m_pNodes[node].PrimPos;
				for (int i = (m_pNodes[node].PrimCount>>3)-1; i >= 0; i--)
				{
					int idTr = offset[i];
					if (a_Ray.TriangleID == idTr)
					{
						continue;
					}
					////m_Candidates++;
					// Check if there is a Triangle Intersection
					a_Ray.TriangleID = idTr;					
					if (RayTriangleTest(a_Ray.o,a_Ray.d,test,idTr,m_pPrimitives))
					{
						if ( test.t < res.t)
						{
							res = test;
							res.IDTr = idTr;
						}
					}
				}
				++mask;
				int p = bsf(mask);
				node=(node>>p)^1;
				mask=(mask>>p);
			}
			else
			{
				//not a leaf, go to first son
				node=node<<1 | signo[m_pNodes[node].PrimCount&3];
				mask=mask<<1;
			}
		}
	}while (mask!=1);
	return res;
}

inline TIntersection LBVH::intersectPnnp(Ray &a_Ray)
{
	TIntersection res(-1,a_Ray.maxt);
	TIntersection test;
	int node=1;
	int mask=1;
	float T0,T1;
	Vector DirectionOver;
	DirectionOver.x=1.0f/a_Ray.d.x;
	DirectionOver.y=1.0f/a_Ray.d.y;
	DirectionOver.z=1.0f/a_Ray.d.z;
	int signo[3]={1,1,0};

	do
	{
		//see if this node intersects with the ray
		RayBoxIntervalnnp(a_Ray.o,DirectionOver,m_pNodes[node],T0,T1);
		//m_Candidates++;
		if ((T1<0.0f)||(T0>res.t))
		{
			//does not intersect, change to next node
			++mask;
			int p = bsf(mask);
			node=(node>>p)^1;
			mask=(mask>>p);
		}
		else
		{
			if ( (m_pNodes[node].PrimCount & 4 )!=0)
			{
				//it is a leaf, compare against all the primitives and asign next node			
				int *offset=m_pPrimitivesIds+m_pNodes[node].PrimPos;
				for (int i = (m_pNodes[node].PrimCount>>3)-1; i >= 0; i--)
				{
					int idTr = offset[i];
					if (a_Ray.TriangleID == idTr)
					{
						continue;
					}
					////m_Candidates++;
					// Check if there is a Triangle Intersection
					a_Ray.TriangleID = idTr;					
					if (RayTriangleTest(a_Ray.o,a_Ray.d,test,idTr,m_pPrimitives))
					{
						if ( test.t < res.t)
						{
							res = test;
							res.IDTr = idTr;
						}
					}
				}
				++mask;
				int p = bsf(mask);
				node=(node>>p)^1;
				mask=(mask>>p);
			}
			else
			{
				//not a leaf, go to first son
				node=node<<1 | signo[m_pNodes[node].PrimCount&3];
				mask=mask<<1;
			}
		}
	}while (mask!=1);
	return res;
}

inline TIntersection LBVH::intersectPnnn(Ray &a_Ray)
{
	TIntersection res(-1,a_Ray.maxt);
	TIntersection test;
	int node=1;
	int mask=1;
	float T0,T1;
	Vector DirectionOver;
	DirectionOver.x=1.0f/a_Ray.d.x;
	DirectionOver.y=1.0f/a_Ray.d.y;
	DirectionOver.z=1.0f/a_Ray.d.z;
	int signo[3]={1,1,1};

	do
	{
		//see if this node intersects with the ray
		RayBoxIntervalnnn(a_Ray.o,DirectionOver,m_pNodes[node],T0,T1);
		//m_Candidates++;
		if ((T1<0.0f)||(T0>res.t))
		{
			//does not intersect, change to next node
			++mask;
			int p = bsf(mask);
			node=(node>>p)^1;
			mask=(mask>>p);
		}
		else
		{
			if ( (m_pNodes[node].PrimCount & 4 )!=0)
			{
				//it is a leaf, compare against all the primitives and asign next node			
				int *offset=m_pPrimitivesIds+m_pNodes[node].PrimPos;
				for (int i = (m_pNodes[node].PrimCount>>3)-1; i >= 0; i--)
				{
					int idTr = offset[i];
					if (a_Ray.TriangleID == idTr)
					{
						continue;
					}
					////m_Candidates++;
					// Check if there is a Triangle Intersection
					a_Ray.TriangleID = idTr;					
					if (RayTriangleTest(a_Ray.o,a_Ray.d,test,idTr,m_pPrimitives))
					{
						if ( test.t < res.t)
						{
							res = test;
							res.IDTr = idTr;
						}
					}
				}
				++mask;
				int p = bsf(mask);
				node=(node>>p)^1;
				mask=(mask>>p);
			}
			else
			{
				//not a leaf, go to first son
				node=node<<1 | signo[m_pNodes[node].PrimCount&3];
				mask=mask<<1;
			}
		}
	}while (mask!=1);
	return res;
}

int bsf( const int x ) { __asm  bsf  eax, x }
int bsr( const int x ) { __asm  bsr  eax, x }

void LBVH::PrintOutput(float &tiempototal)
{
	float structureSize = static_cast<float>(m_iNumNodes*sizeof(LBVHNode))/static_cast<float>(1048576);
	int numLeafs = static_cast<int>(m_iNumNodes/2);
	float occupiedLeafsRate = (static_cast<float>(occupiedLeafs)/static_cast<float>(numLeafs))*100;
	int emptyLeafs = numLeafs - occupiedLeafs;

	printf(
		"LBVH: %d Nodes * %d Bytes per Node = %.2f MB\n"
		"LBVH: Occupied Nodes = %d, Free Nodes = %d\n"
		"LBVH: Total Leafs = %d, Occupied Leafs = %d (%.2f), Empty Leafs = %d\n"
		"LBVH: Primitives/Leaf = %d/%d (%lf)\n",
		m_iNumNodes, sizeof(LBVHNode), structureSize, OccupiedNodes, m_iNumNodes-OccupiedNodes, 
		numLeafs, occupiedLeafs, occupiedLeafsRate, emptyLeafs,this->m_uiNumPrimitives,occupiedLeafs,double(this->m_uiNumPrimitives)/double(occupiedLeafs));
}
