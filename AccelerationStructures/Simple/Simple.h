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

#ifndef SIMPLE_H
#define SIMPLE_H

#include "AccelerationStructure.h"
#include "BBox.h"

class Simple : public AccelerationStructure
{
private:

public:
	Simple(Primitive** a_Primitive = NULL, unsigned int a_uiNumPrimitives = 0);
	~Simple()
	{
	}

	unsigned int				GetNumberOfElements() { return 0; }

	TIntersection IntersectP(Ray &a_Ray)
	{
		TIntersection res(-1,a_Ray.maxt);
		for (unsigned int current=0; current < m_uiNumPrimitives; current++)
		{
			/*if (m_MailBox[current] == MailBoxId)
			{
				continue;
			}*/
			//m_Candidates++;
			TIntersection test;
			if (RayTriangleTest(a_Ray.o,a_Ray.d,test,current,m_pPrimitives) && test.t < res.t)
			{
					res = test;
					res.IDTr = current;
			}
		}
		return res;
	}
	
	void Build() {}
	size_t				GetSizeOfElement() { return 0; }
	void*				GetPtrToElements() { return static_cast<void*>(NULL); }
	unsigned int*		GetOrderedElementsIds() { return 0; }
	unsigned int		GetKeyboardId() { return 5;}
	BBox				WorldBound() { return BBox(); }

	void PrintOutput(float &tiempototal)
	{
		char aux[1024];
#ifdef WINDOWS
		sprintf_s(aux,sizeof(aux)*1024,"Tiempo en generar estructuras: %f segundos\n",tiempototal);
#elif defined(LINUX)
		sprintf(aux,"Tiempo en generar estructuras: %f segundos\n",tiempototal);
#endif
	}
};

#endif