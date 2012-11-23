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

#ifndef ACCELERATIONSTRUCTURE_H
#define ACCELERATIONSTRUCTURE_H

#include "Geometry.h"
#include "Primitive.h"

#ifndef ACCELERATION_STRUCTURE
enum ACCELERATION_STRUCTURE
{
    AS_NULLSTRUCT,
    AS_SIMPLE,
    AS_LBVH,
    AS_BIH,
    AS_KDTREE,
    AS_GRID,
    AS_BVH,
	AS_BLBVH
};

inline ACCELERATION_STRUCTURE operator++( ACCELERATION_STRUCTURE &as, int ) 
{
   return as = (ACCELERATION_STRUCTURE)(as + 1);
}
#endif

class AccelerationStructure
{
public:
	AccelerationStructure(void);
	virtual ~AccelerationStructure(void) = 0;
	char* GetName() { return m_sName; }
	
	virtual TIntersection IntersectP(Ray &a_Ray) = 0;
	virtual void PrintOutput(float &totaltime) = 0;
	virtual void Build() = 0;
	virtual unsigned int GetNumberOfElements() = 0;
	virtual size_t GetSizeOfElement() = 0;
	virtual void* GetPtrToElements() = 0;
	virtual unsigned int* GetOrderedElementsIds() = 0;
	virtual unsigned int GetKeyboardId() = 0;
	virtual BBox WorldBound() = 0; 

	Primitive**		GetPrimitives() { return m_pPrimitives; }
	unsigned int	GetNumPrimitives() { return m_uiNumPrimitives; }
	int				GetInitNode() { return m_iInitNode; }
	unsigned int	GetCandidates() { return m_uiCandidates; }
protected:
	Primitive**		m_pPrimitives;
	unsigned int	m_uiNumPrimitives;
	char*			m_sName;
	int				m_iInitNode;
	unsigned int	m_uiCandidates;
};

//Tests if a Ray Intersects a triangle
inline bool RayTriangleTest(Point &Start,Vector &Direction,TIntersection &intersection, unsigned int current, Primitive** a_Primitives)
{	
	Point A(a_Primitives[current]->GetVertex(0)->Pos);
	Point B(a_Primitives[current]->GetVertex(1)->Pos);
	Point C(a_Primitives[current]->GetVertex(2)->Pos);

	Vector E1(B-A);
	Vector E2(C-A);

	Vector P, Q;
	Cross(P, Direction, E2);
	float det;
	Dot(det, E1,P);
	det = 1.0f/det;

	Vector T(Start - A);
	float res;
	Dot(res,T,P);
	intersection.u = res*det;
	Cross(Q, T, E1);
	Dot(intersection.v, Direction,Q);
	intersection.v *= det;
	Dot(intersection.t, E2,Q);
	intersection.t *= det;
	
	//there is a hit
	return ((intersection.u>=0.0f)&&(intersection.u<=1.0f)&&(intersection.v>=0.0f)&&((intersection.u+intersection.v)<=1.0f)&&(intersection.t>=0.0f));
}

#endif