//////////////////////////////////////////////////////////////////////
//
//  TranslucentPelPhotonMapShaderOp.cpp - Implementation of the 
//    TranslucentPelPhotonMapShaderOp class
//
//  Author: Aravind Krishnaswamy
//  Date of Birth: January 28, 2005
//  Tabs: 4
//  Comments:  
//
//  License Information: Please see the attached LICENSE.TXT file
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "TranslucentPelPhotonMapShaderOp.h"
#include "../Utilities/GeometricUtilities.h"

using namespace RISE;
using namespace RISE::Implementation;

TranslucentPelPhotonMapShaderOp::TranslucentPelPhotonMapShaderOp(
	)
{
}

TranslucentPelPhotonMapShaderOp::~TranslucentPelPhotonMapShaderOp( )
{
}

//! Tells the shader to apply shade to the given intersection point
void TranslucentPelPhotonMapShaderOp::PerformOperation(
	const RuntimeContext& rc,					///< [in] Runtime context
	const RayIntersection& ri,					///< [in] Intersection information 
	const IRayCaster& caster,					///< [in] The Ray Caster to use for all ray casting needs
	const IRayCaster::RAY_STATE& rs,			///< [in] Current ray state
	RISEPel& c,									///< [in/out] Resultant color from op
	const IORStack* const ior_stack,			///< [in/out] Index of refraction stack
	const ScatteredRayContainer* pScat			///< [in] Scattering information
	) const
{
	c = RISEPel(0.0);

	// Only do stuff on a normal pass or on final gather
	if( rc.pass != RuntimeContext::PASS_NORMAL && rs.type == rs.eRayView ) {
		return;
	}

	const IScene* pScene = caster.GetAttachedScene();
	const IPhotonMap* pTM = pScene->GetTranslucentPelMap();
	const IBSDF* pBRDF = ri.pMaterial->GetBSDF();

	if( pTM && pBRDF ) {
		pTM->RadianceEstimate( c, ri.geometric, *pBRDF );
	}
}

//! Tells the shader to apply shade to the given intersection point for the given wavelength
/// \return Amplitude of spectral function 
Scalar TranslucentPelPhotonMapShaderOp::PerformOperationNM(
	const RuntimeContext& rc,					///< [in] Runtime context
	const RayIntersection& ri,					///< [in] Intersection information 
	const IRayCaster& caster,					///< [in] The Ray Caster to use for all ray casting needs
	const IRayCaster::RAY_STATE& rs,			///< [in] Current ray state
	const Scalar caccum,						///< [in] Current value for wavelength
	const Scalar nm,							///< [in] Wavelength to shade
	const IORStack* const ior_stack,			///< [in/out] Index of refraction stack
	const ScatteredRayContainer* pScat			///< [in] Scattering information
	) const
{
	// Invalid operation!
	return 0;
}
