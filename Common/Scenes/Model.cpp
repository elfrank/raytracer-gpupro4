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

#include "ArgumentsParser.h"
#include "Model.h"
#include <vector>

extern ArgumentsParser m_Parser;

class KdTree 
{
public:
   Vertex *V;
   unsigned int Index;
   KdTree *Left, *Right;

public:
	KdTree() 
	{
		V = NULL;
		Left = Right = NULL;
		Index = 0;
	}

	KdTree(Vertex *in_V, unsigned int in_Index)
	{
		V = in_V;
		Index = in_Index;
		Left = NULL;
		Right = NULL;
	}

	~KdTree()
	{
		if (NULL != Left) delete Left;
		if (NULL != Right) delete Right;
		//delete V;
	}
};

unsigned int g_uiNextIndex = 0;
KdTree *g_pKdTree = NULL;
list<unsigned int> *g_IList = NULL;

KdTree *KdTreeInsert(Vertex *in_V, unsigned int in_Index, KdTree *Tree, int Level, KdTree **out_oTreeFound)
{
	if (NULL == Tree)
	{
		*out_oTreeFound = NULL;
		return new KdTree(in_V, in_Index);
	}
	
	if (Tree->V->Pos.x == in_V->Pos.x &&
		Tree->V->Pos.y == in_V->Pos.y &&
		Tree->V->Pos.z == in_V->Pos.z &&
		Tree->V->U == in_V->U &&
		Tree->V->V == in_V->V &&
		Tree->V->Normal.x == in_V->Normal.x &&
		Tree->V->Normal.y == in_V->Normal.y &&
		Tree->V->Normal.z == in_V->Normal.z)
	{
		// Vertex already in tree
		*out_oTreeFound = Tree;
		return Tree; 
	}

	float fCmp;
	switch (Level)
	{
		case 0:
			fCmp = Tree->V->Pos.x - in_V->Pos.x;
			break;
		case 1:
			fCmp = Tree->V->Pos.y - in_V->Pos.y;
			break;
		case 2:
			fCmp = Tree->V->Pos.z - in_V->Pos.z;
			break;
	}

	if (fCmp < 0.0f)
	{
		Tree->Left = KdTreeInsert(in_V, in_Index, Tree->Left, (Level + 1) % 3, out_oTreeFound);
		return Tree;
	}
	else
	{
		Tree->Right = KdTreeInsert(in_V, in_Index, Tree->Right, (Level + 1) % 3, out_oTreeFound);
		return Tree;
	}
}

//------------------------------------------------
// Constructor
//------------------------------------------------
Model::Model( string sFileName )
{
	size_t pos = sFileName.find(".");
	m_sName = sFileName.substr(0,pos);

	LoadFile(sFileName);	
	Scale();
	// The actual threshold is: fThreshold * MaxArea. Anything above this value is tessellated recursively.
	//Tessellate(0.01f);

	m_pAccelStructure = new NullShader(m_ppPrimitives, m_uiNumPrimitives);
	if(strcmp(m_Parser.GetAccelerationStructure(),"bvh") == 0)
	{
		SetCurrentStructureType(AS_BVH);	
	}
	else if(strcmp(m_Parser.GetAccelerationStructure(),"lbvh") == 0)
	{
		SetCurrentStructureType(AS_LBVH);
	}
	else if(strcmp(m_Parser.GetAccelerationStructure(),"bbvh") == 0)
	{
		SetCurrentStructureType(AS_BLBVH);
	}
	else
	{
		printf("ERROR-> Wrong acceleration structure input.\nLoading BVH.\n");	
		SetCurrentStructureType(AS_BVH);	
	}
}

//------------------------------------------------
// Destructor
//------------------------------------------------
Model::~Model()
{
	for(unsigned int i = 0; i < m_uiNumPrimitives; ++i) SAFE_DELETE(m_ppPrimitives[i]);
	for(unsigned int i = 0; i < m_uiNumMaterials; ++i) SAFE_DELETE(m_ppMaterials[i]);
	SAFE_DELETE(m_ppPrimitives);
	SAFE_DELETE(m_ppMaterials);
	SAFE_DELETE(m_pAccelStructure);
	SAFE_DELETE(m_pVertices);
	SAFE_DELETE(m_pIndices);
}

//------------------------------------------------
// Change structure for the model and rebuild it
//------------------------------------------------
void Model::SetCurrentStructureType(ACCELERATION_STRUCTURE eStructureType) 
{	
	m_eAccelStructureType = eStructureType;
	RebuildStructure();
}

//------------------------------------------------
// Build new acceleration structure
//------------------------------------------------
void Model::RebuildStructure()
{	
	if( m_pAccelStructure )
	{
		delete m_pAccelStructure;
	}

	switch(m_eAccelStructureType)
	{
	case AS_BVH:
		m_pAccelStructure = new BVH(m_ppPrimitives, m_uiNumPrimitives, m_Parser.GetMaxPrimsInNode(), m_Parser.GetBVHSplit());
		break;
	case AS_LBVH:
		m_pAccelStructure = new LBVH(m_ppPrimitives, m_uiNumPrimitives, 1 << m_Parser.GetLBVHDepth() );
		break;
	case AS_BIH:
		m_pAccelStructure = new BIH(m_ppPrimitives, m_uiNumPrimitives);
		break;
	case AS_KDTREE:
		m_pAccelStructure = new KDTree(m_ppPrimitives, m_uiNumPrimitives);
		break;
	case AS_GRID:
		m_pAccelStructure = new CubicGrid(m_ppPrimitives, m_uiNumPrimitives);
		break;
	case AS_SIMPLE:
		m_pAccelStructure = new Simple(m_ppPrimitives, m_uiNumPrimitives);
		break;
	default:
		m_pAccelStructure = new NullShader(m_ppPrimitives, m_uiNumPrimitives);
		break;
	}

	LARGE_INTEGER LocalTimer, Freq;
	startTimer(LocalTimer, Freq);

	m_pAccelStructure->Build();

	float totalTime;
	calculateTime(LocalTimer, Freq, totalTime);
	printf("%s Build Time: %f seconds\n",m_pAccelStructure->GetName(), totalTime);

	m_pAccelStructure->PrintOutput(totalTime);
}

//------------------------------------------------
// Load model file
//------------------------------------------------
void Model::LoadFile(string sFileName)
{
	const string sPath = "./Models/";
	const string sFilePath = sPath + sFileName;
	
	if (strstr(sFileName.c_str(), ".txt") || strstr(sFileName.c_str(), ".TXT"))
	{
		FILE *file = NULL;
		fopen_s(&file,sFilePath.c_str(),"rt");
		if(file == NULL)
		{
			printf("Could not open the file %s\n", sFilePath.c_str());
			exit(EXIT_FAILURE);
		}

		fscanf_s(file,"%d",&m_uiNumVertices);
		fscanf_s(file,"%d",&m_uiNumPrimitives);
		unsigned int numIndices = m_uiNumPrimitives*3;
		m_ppPrimitives = new Primitive*[m_uiNumPrimitives];
		m_pVertices = new Vertex[m_uiNumVertices];
		m_pIndices = new unsigned long[numIndices];
		m_uiNumMaterials = 1;
		m_ppMaterials = new Material*[m_uiNumMaterials];
		m_ppMaterials[0] = new Material(0, "default.jpg");
		for (unsigned int i = 0; i < m_uiNumVertices; i++)
		{
			fscanf_s(file,"%f %f %f %f %f %f %f %f",
				&m_pVertices[i].Pos.x,&m_pVertices[i].Pos.y,&m_pVertices[i].Pos.z,
				&m_pVertices[i].Normal.x,&m_pVertices[i].Normal.y,&m_pVertices[i].Normal.z,
				&m_pVertices[i].U,&m_pVertices[i].V);
			Normalize(m_pVertices[i].Normal);

			float x, y, z;
			x = m_pVertices[i].Pos.x;
			y = m_pVertices[i].Pos.y;
			z = m_pVertices[i].Pos.z;
			float r = sqrtf(x*x+y*y+z*z);
			m_pVertices[i].U = x*7;
			m_pVertices[i].V = y*7;
		} // END for

		int tmpOffset = 0;
		for (unsigned int i = 0; i < m_uiNumPrimitives; i++)
		{
			int a,b,c;

			fscanf_s(file,"%d %d %d",&a,&b,&c);
			m_pIndices[tmpOffset] = a;
			m_pIndices[tmpOffset+1] = b;
			m_pIndices[tmpOffset+2] = c;
			tmpOffset += 3;
			m_ppPrimitives[i] = new Primitive(&m_pVertices[a], &m_pVertices[b], &m_pVertices[c]);
			m_ppPrimitives[i]->SetMaterial(m_ppMaterials[0]);
		} // END for

		fclose(file);
	}
	else if (strstr(sFileName.c_str(), ".obj") || strstr(sFileName.c_str(), ".OBJ"))
	{
		if (!m_rModelObj.import(sFilePath.c_str()))
		{
			//throw std::runtime_error("Failed to load model.");
			printf("Could not open the model %s\n", sFilePath.c_str());
			exit(EXIT_FAILURE);
		}
		m_rModelObj.normalize();

		m_uiNumVertices = m_rModelObj.getNumberOfVertices();
		m_uiNumPrimitives = m_rModelObj.getNumberOfTriangles();
		m_uiNumMaterials = m_rModelObj.getNumberOfMaterials();

		printf("Parsing model to RT format...\n");
		printf("m_uiNumVertices: %d\n", m_uiNumVertices);
		printf("m_uiNumPrimitives: %d\n", m_uiNumPrimitives);
		printf("m_uiNumMaterials: %d\n", m_uiNumMaterials);

		if(m_uiNumMaterials > 0)
		{
			m_ppMaterials = new Material*[m_uiNumMaterials];
			m_ppMaterialsData = new MaterialData[m_uiNumMaterials];
			for(unsigned int i = 0; i < m_uiNumMaterials; ++i)
			{
				m_ppMaterialsData[i].SetDiffuse(&m_rModelObj.getMaterial(i).diffuse[0]);
				m_ppMaterialsData[i].SetAmbient(m_rModelObj.getMaterial(i).ambient);
				m_ppMaterialsData[i].SetSpecular(m_rModelObj.getMaterial(i).specular);
				m_ppMaterialsData[i].SetAlpha(m_rModelObj.getMaterial(i).alpha);
				m_ppMaterialsData[i].SetShininess(m_rModelObj.getMaterial(i).shininess);

				std::string texturePath = m_rModelObj.getMaterial(i).colorMapFilename;
				if(texturePath == "")
				{
					texturePath = "default.jpg";
				}
				m_ppMaterials[i] = new Material( i, texturePath , m_ppMaterialsData );
			}
		}
		else
		{
			m_uiNumMaterials = 1;
			m_ppMaterials = new Material*[m_uiNumMaterials];
			m_ppMaterialsData = new MaterialData[m_uiNumMaterials];

			/*m_ppMaterialsData[0].SetDiffuse(&m_rModelObj.getMaterial(i).diffuse[0]);
			m_ppMaterialsData[0].SetAmbient(m_rModelObj.getMaterial(i).ambient);
			m_ppMaterialsData[0].SetSpecular(m_rModelObj.getMaterial(i).specular);
			m_ppMaterialsData[0].SetAlpha(m_rModelObj.getMaterial(i).alpha);
			m_ppMaterialsData[0].SetShininess(m_rModelObj.getMaterial(i).shininess);*/
			
			m_ppMaterials[0] = new Material( 0, "default.jpg", m_ppMaterialsData );
		}
	
		printf("Parsing vertices...\n");
		m_pVertices = new Vertex[m_uiNumVertices];
		for(unsigned int i = 0; i < m_uiNumVertices; ++i)
		{
			m_pVertices[i].Pos.x = m_rModelObj.getVertexBuffer()[i].position[0];
			m_pVertices[i].Pos.y = m_rModelObj.getVertexBuffer()[i].position[1];
			m_pVertices[i].Pos.z = m_rModelObj.getVertexBuffer()[i].position[2];

			m_pVertices[i].Normal.x = m_rModelObj.getVertexBuffer()[i].normal[0];
			m_pVertices[i].Normal.y = m_rModelObj.getVertexBuffer()[i].normal[1];
			m_pVertices[i].Normal.z = m_rModelObj.getVertexBuffer()[i].normal[2];

			m_pVertices[i].U = m_rModelObj.getVertexBuffer()[i].texCoord[0];
			m_pVertices[i].V = -m_rModelObj.getVertexBuffer()[i].texCoord[1];
		}

		unsigned int uiNumIndices = m_uiNumPrimitives*3;
		m_pIndices = new DWORD[uiNumIndices];
		m_ppPrimitives = new Primitive*[m_uiNumPrimitives];
		int tmpOffset = 0;
		
		printf("Parsing primitives...\n");
		for(unsigned int i = 0; i < m_uiNumPrimitives; ++i)
		{
			int a,b,c;
			a = m_rModelObj.getIndexBuffer()[i*3];
			b = m_rModelObj.getIndexBuffer()[i*3+1];
			c = m_rModelObj.getIndexBuffer()[i*3+2];

			m_pIndices[tmpOffset] = a;
			m_pIndices[tmpOffset+1] = b;
			m_pIndices[tmpOffset+2] = c;
			tmpOffset += 3;

			m_ppPrimitives[i] = new Primitive(&m_pVertices[a], &m_pVertices[b], &m_pVertices[c]);
			if(m_uiNumMaterials == 1) 
				m_ppPrimitives[i]->SetMaterial(m_ppMaterials[0]);
		}

		printf("Parsing materials...\n");
		if(m_uiNumMaterials>1)
		{
			printf("Number of meshes: %d\n", m_rModelObj.getNumberOfMeshes());
			for(int i = 0; i < m_rModelObj.getNumberOfMeshes(); ++i)
			{
				int numTriangles = m_rModelObj.getMesh(i).triangleCount;
				int startIndex = m_rModelObj.getMesh(i).startIndex/3;
				int materialId = m_rModelObj.getMaterialCache()[m_rModelObj.getMesh(i).pMaterial->name];

				Material* currentMat = m_ppMaterials[materialId];
				for(int j = 0; j < numTriangles; ++j)
				{
					m_ppPrimitives[startIndex+j]->SetMaterial(currentMat);
				}
			}
		}
	}

	printf("m_uiNumMaterials (final): %d\n", m_uiNumMaterials);
	printf("%s imported successfully.\n",sFileName.c_str());
}

float Model::GetPrimitiveArea(unsigned int iNumPrimitive)
{
	Vector D1, D2, C;
	float Area;

	Vertex *v0 = m_ppPrimitives[iNumPrimitive]->GetVertex(0);
	Vertex *v1 = m_ppPrimitives[iNumPrimitive]->GetVertex(1);
	Vertex *v2 = m_ppPrimitives[iNumPrimitive]->GetVertex(2);

	D1.x = v1->Pos.x - v0->Pos.x;
	D1.y = v1->Pos.y - v0->Pos.y;
	D1.z = v1->Pos.z - v0->Pos.z;

	D2.x = v2->Pos.x - v0->Pos.x;
	D2.y = v2->Pos.y - v0->Pos.y;
	D2.z = v2->Pos.z - v0->Pos.z;
	Cross(C, D1,D2);

	Length(Area, C);
	return Area;
}

void Model::Tessellate(float fThreshold)
{
	// First pass: find the area of the smallest primitive
	float MinArea = 1000.0f;
	float MaxArea = 0.0f;
	for (unsigned int i = 0; i < m_uiNumPrimitives; i++)
	{
		float Area = GetPrimitiveArea(i);
		if (Area != 0.0f)
		{
			if (Area > MaxArea) MaxArea = Area;
			if (Area < MinArea) MinArea = Area;
		}
		
	}

	// printf("Min, Max area: %0.6f, %0.6f\n", MinArea, MaxArea);
	printf("Tessellation threshold: MaxArea * %0.6f\n", fThreshold);
	float MaxThreshold = MaxArea * fThreshold;
	list<Vertex*> *VList = new list<Vertex*>();
	list<Material*> *MList = new list<Material*>();
	g_IList = new list<unsigned int>();
	g_uiNextIndex = 0;
	for (unsigned int i = 0; i < m_uiNumPrimitives; i++)
	{
		float Area = GetPrimitiveArea(i);
		if (Area != 0.0f)
		{
			// We need to make copies of the original values; because we'll be re-arranging things around.
			Vertex v0 = *(m_ppPrimitives[i]->GetVertex(0));
			Vertex v1 = *(m_ppPrimitives[i]->GetVertex(1));
			Vertex v2 = *(m_ppPrimitives[i]->GetVertex(2));
			Material *M = m_ppPrimitives[i]->GetMaterial();

			RecTessellate(VList, MList, v0, v1, v2, M, MaxThreshold);
		}
	}
	printf("%d vertices after tessellation.\n", VList->size());
	printf("%d primitives after tessellation.\n", MList->size());

	unsigned int uiNumVertices = VList->size();
	unsigned int uiNumPrimitives = MList->size();
	Primitive **ppPrimitives = new Primitive*[uiNumPrimitives];
	Vertex *pVertices = new Vertex[uiNumVertices];
	unsigned long *pIndices = new unsigned long[g_IList->size()];
	unsigned int i;
	unsigned int Idx, Ofs;	
	list<Vertex*>::iterator it;
	list<Material*>::iterator mit;

	// Copy the list of vertices
	for (i = 0, it = VList->begin(); i < uiNumVertices; i++, it++)
	{
		pVertices[i] = *(*it);
	}

	// Copy the indices and re-create the primitives
	list<unsigned int>::iterator uiIt = g_IList->begin();
	for (i = 0, Idx = 0, Ofs = 0, mit = MList->begin(); i < uiNumPrimitives; i++, mit++, Idx += 3, Ofs += 3)
	{
		unsigned int a = *uiIt; uiIt++;
		unsigned int b = *uiIt; uiIt++;
		unsigned int c = *uiIt; uiIt++;

		pIndices[Ofs]	  = a;
		pIndices[Ofs + 1] = b;
		pIndices[Ofs + 2] = c;

		ppPrimitives[i] = new Primitive(&pVertices[a], &pVertices[b], &pVertices[c]);
		ppPrimitives[i]->SetMaterial( *mit );
		*mit = NULL;
	}

	// Erase temporary data
	delete g_pKdTree;
	VList->clear();
	MList->clear();
	g_IList->clear();
	delete VList;
	delete MList;
	delete g_IList;

	// Replace old geometry with tessellated geometry.
	delete[] m_pVertices;
	delete[] m_pIndices;
	delete[] m_ppPrimitives;
	
	m_pVertices = pVertices;
	m_pIndices = pIndices;
	m_ppPrimitives = ppPrimitives;
	m_uiNumPrimitives = uiNumPrimitives;
	m_uiNumVertices = uiNumVertices;
}

void Model::RecTessellate(list<Vertex*> *inout_VList, list<Material*> *inout_MList, Vertex v0, Vertex v1, Vertex v2, Material *M, float fThreshold)
{
	Vector D1, D2, D3, C;
	float Area;
	Vertex v3;

	// Compute the area of the current triangle
	D1.x = v1.Pos.x - v0.Pos.x;
	D1.y = v1.Pos.y - v0.Pos.y;
	D1.z = v1.Pos.z - v0.Pos.z;

	D2.x = v2.Pos.x - v0.Pos.x;
	D2.y = v2.Pos.y - v0.Pos.y;
	D2.z = v2.Pos.z - v0.Pos.z;

	D3.x = v2.Pos.x - v1.Pos.x;
	D3.y = v2.Pos.y - v1.Pos.y;
	D3.z = v2.Pos.z - v1.Pos.z;

	Cross(C, D1,D2);
	Length(Area, C);

	// Tessellate if necessary
	if (Area <= fThreshold)
	{
		Vertex *pV0 = new Vertex();
		Vertex *pV1 = new Vertex();
		Vertex *pV2 = new Vertex();

		Normalize(v0.Normal);
		Normalize(v1.Normal);
		Normalize(v2.Normal);

		*pV0 = v0;
		*pV1 = v1;
		*pV2 = v2;		

		/*
		float N0, N1, N2;
		Length(N0, v0.Normal);
		Length(N1, v1.Normal);
		Length(N2, v2.Normal);
		if (N0 < 0.00001f || N1 < 0.00001f || N2 < 0.00001f)
		{
			printf("NULL NORMAL!\n");
		}
		*/

		// Check each vertex for duplicity before adding them to the vertex list
		KdTree *pTreeFound;
		
		pTreeFound = NULL;
		g_pKdTree = KdTreeInsert(pV0, g_uiNextIndex, g_pKdTree, 0, &pTreeFound);
		if (NULL == pTreeFound)
		{
			inout_VList->push_back(pV0);
			g_IList->push_back(g_uiNextIndex);
			g_uiNextIndex++;
		}
		else
		{
			delete pV0;
			//printf("Reused vertex\n");
			//inout_VList->push_back(pTreeFound->V);
			g_IList->push_back(pTreeFound->Index);
		}
		
		pTreeFound = NULL;
		g_pKdTree = KdTreeInsert(pV1, g_uiNextIndex, g_pKdTree, 0, &pTreeFound);
		if (NULL == pTreeFound)
		{
			inout_VList->push_back(pV1);
			g_IList->push_back(g_uiNextIndex);
			g_uiNextIndex++;
		}
		else
		{
			delete pV1;
			//printf("Reused vertex\n");
			//inout_VList->push_back(pTreeFound->V);
			g_IList->push_back(pTreeFound->Index);
		}

		pTreeFound = NULL;
		g_pKdTree = KdTreeInsert(pV2, g_uiNextIndex, g_pKdTree, 0, &pTreeFound);
		if (NULL == pTreeFound)
		{
			inout_VList->push_back(pV2);
			g_IList->push_back(g_uiNextIndex);
			g_uiNextIndex++;
		}
		else
		{
			delete pV2;
			//printf("Reused vertex\n");
			//inout_VList->push_back(pTreeFound->V);
			g_IList->push_back(pTreeFound->Index);
		}

		inout_MList->push_back(M);
	}
	else
	{
		float LD1, LD2, LD3;
		//list<Vertex> *VList1 = NULL;
		//list<Vertex> *VList2 = NULL;

		Length(LD1, D1);
		Length(LD2, D2);
		Length(LD3, D3);
		float Max = max(max(LD1, LD2), LD3);
		float N;
		if (Max == LD1)
		{
			// LD1: v1, v0
			v3.Pos	  = (v1.Pos	    + v0.Pos) / 2.0f;
			v3.Normal = (v1.Normal  + v0.Normal) / 2.0f;
			v3.U	  = (v1.U		+ v0.U) / 2.0f;
			v3.V	  = (v1.V		+ v0.V) / 2.0f;
			RecTessellate(inout_VList, inout_MList, v0, v3, v2, M, fThreshold);
			RecTessellate(inout_VList, inout_MList, v3, v1, v2, M, fThreshold);
		}
		else if (Max = LD2)
		{
			// LD2: v2, v0
			v3.Pos	  = (v2.Pos	    + v0.Pos) / 2.0f;
			v3.Normal = (v2.Normal  + v0.Normal) / 2.0f;
			v3.U	  = (v2.U		+ v0.U) / 2.0f;
			v3.V	  = (v2.V		+ v0.V) / 2.0f;
			RecTessellate(inout_VList, inout_MList, v0, v1, v3, M, fThreshold);
			RecTessellate(inout_VList, inout_MList, v1, v2, v3, M, fThreshold);
		}
		else
		{
			// LD3: v2, v1
			v3.Pos	  = (v2.Pos	    + v1.Pos) / 2.0f;
			v3.Normal = (v2.Normal  + v1.Normal) / 2.0f;
			v3.U	  = (v2.U		+ v1.U) / 2.0f;
			v3.V	  = (v2.V		+ v1.V) / 2.0f;
			RecTessellate(inout_VList, inout_MList, v0, v1, v3, M, fThreshold);
			RecTessellate(inout_VList, inout_MList, v3, v2, v0, M, fThreshold);
		}
	}
}

//------------------------------------------------
// Scale all models to fit a 0.8 radius sphere
//------------------------------------------------
void Model::Scale()
{
	printf("Scale scene...\n");
	// scale everything to fit the 0.8 radius sphere
	Vector3 MinVec = m_pVertices[0].Pos,MaxVec = m_pVertices[0].Pos;
	for (unsigned int i=1;i<m_uiNumVertices;i++)
	{
		if (m_pVertices[i].Pos.x < MinVec.x)
		{
			MinVec.x=m_pVertices[i].Pos.x;
		}
		if (m_pVertices[i].Pos.x>MaxVec.x)
		{
			MaxVec.x=m_pVertices[i].Pos.x;
		}

		if (m_pVertices[i].Pos.y<MinVec.y)
		{
			MinVec.y=m_pVertices[i].Pos.y;
		}
		if (m_pVertices[i].Pos.y>MaxVec.y)
		{
			MaxVec.y=m_pVertices[i].Pos.y;
		}

		if (m_pVertices[i].Pos.z<MinVec.z)
		{
			MinVec.z=m_pVertices[i].Pos.z;
		}
		if (m_pVertices[i].Pos.z>MaxVec.z)
		{
			MaxVec.z=m_pVertices[i].Pos.z;
		}
	}
	MaxVec = MaxVec - MinVec;
	float scale = 1.8f/max(MaxVec.x,max(MaxVec.y,MaxVec.z));
	MinVec= MinVec+(0.5f*MaxVec);
	for (int i=m_uiNumVertices-1;i>=0;i--)
	{
		m_pVertices[i].Pos=m_pVertices[i].Pos-MinVec;
		m_pVertices[i].Pos=scale*m_pVertices[i].Pos;
	}

	// scale again but now to fit the 0.49 radius sphere
	scale = 0.0f;
	for (unsigned int i=0;i<m_uiNumVertices;i++)
	{
		float lng;
		Dot(lng, m_pVertices[i].Pos, m_pVertices[i].Pos);
		
		if (lng>scale)
		{
			scale = lng;
		}
	}
	scale=sqrt(scale);
	scale=0.49f/scale;
	for (int i=m_uiNumVertices-1;i>=0;i--)
	{
		m_pVertices[i].Pos.x*=scale;
		m_pVertices[i].Pos.y*=scale;
		m_pVertices[i].Pos.z*=scale;
	}
}