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

#ifndef __ARG_PARSER_H__
#define __ARG_PARSER_H__

#include <glibmm.h>
#include <iostream>
#ifdef WINDOWS
#include "WindowsUtil.h"
#elif defined (LINUX)
#include <DataTypes.h>
#endif

class ArgumentsParser : public Glib::OptionGroup
{
private:
	Glib::OptionContext		m_OptionContext; // context of glib
	GKeyFile*				m_pKeyFile; // ini file to load
	GError*					m_pError; // error pointer

	// Global
	int						m_iNumThreads; // number of threads used by the cpu
	int						m_iIterations; // number of frames rendered before quitting
	
	// Camera-Model variables
	Glib::ustring			m_sModel; // model to load
	float					m_fSpeed; // camera speed
	
	// Reflections
	int						m_iNumReflections; // number of inital reflections
	bool					m_bIsReflective; // reflections are on?
	bool					m_bIsMultiplicative; // are reflections multiplicative?
	
	// Acceleration Structures
	Glib::ustring			m_sAccelerationStruct; // name of the acceleration structure
	Glib::ustring			m_sBVHSplit; // split algorithm for the BVH
	int						m_iMaxPrimsNode; // maximum number of primitives per node
	int						m_iLBVHDepth; // LBVH depth of the tree

	// Screen
	int						m_iTextureWidth; // width of the render target texture
	int						m_iTextureHeight; // height of the render target texture
	int						m_iScreenMultiplier; // multiplier for textures

	// GPU execution
	int						m_iGroupSizeX; // number of groups in x-dim to dispatch in GPU
	int						m_iGroupSizeY; // number of groups in x-dim to dispatch in GPU
	int						m_iGroupSizeZ; // number of groups in x-dim to dispatch in GPU
	bool					m_bIsDynamic; // the model can be either static or dynamic (reconstruction of the acceleration structure)


public:
	ArgumentsParser();
	~ArgumentsParser();

	// Glib functions
	virtual bool			on_pre_parse(Glib::OptionContext& context, Glib::OptionGroup& group);
	virtual bool			on_post_parse(Glib::OptionContext& context, Glib::OptionGroup& group);
	virtual void			on_error(Glib::OptionContext& context, Glib::OptionGroup& group);

	// Parser functions
	int						ParseData();
	int						LoadConfigurationFromFile(const char* sFile);
	void					ShowConfiguration(unsigned int uiProcesses);
	
	// Getters
	unsigned int			GetNumThreads() { return static_cast<unsigned int>(m_iNumThreads); }
	unsigned int			GetNumReflections() { return static_cast<unsigned int>(m_iNumReflections); }
	unsigned int			GetMaxPrimsInNode() { return static_cast<unsigned int>(m_iMaxPrimsNode); }
	unsigned int			GetTextureWidth() { return static_cast<unsigned int>(m_iTextureWidth); }
	unsigned int			GetTextureHeight() { return static_cast<unsigned int>(m_iTextureHeight); }
	unsigned int			GetScreenMultiplier() { return static_cast<unsigned int>(m_iScreenMultiplier); }
	unsigned int			GetGroupSizeX() { return static_cast<unsigned int>(m_iGroupSizeX); }
	unsigned int			GetGroupSizeY() { return static_cast<unsigned int>(m_iGroupSizeY); }
	unsigned int			GetGroupSizeZ() { return static_cast<unsigned int>(m_iGroupSizeZ); }
	unsigned int			GetNumIterations() { return static_cast<unsigned int>(m_iIterations); }
	unsigned int			GetLBVHDepth() { return static_cast<unsigned int>(m_iLBVHDepth); }
	float					GetSpeed() { return m_fSpeed; }
	const char*				GetModelName() { return m_sModel.c_str(); }
	const char*				GetBVHSplit() { return m_sBVHSplit.c_str(); }
	const char*				GetAccelerationStructure() { return m_sAccelerationStruct.c_str(); }
	bool					IsReflective() { return m_bIsReflective; }
	bool					IsMultiplicative() { return m_bIsMultiplicative; }
	bool					IsDynamic() { return m_bIsDynamic; }
};

#endif //__ARG_PARSER_H__
