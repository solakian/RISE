//////////////////////////////////////////////////////////////////////
//
//  ReflectionShaderOp.h - The reflection shader op follows all
//    scattered reflection rays.
//
//  Author: Aravind Krishnaswamy
//  Date of Birth: January 28, 2005
//  Tabs: 4
//  Comments:  
//
//  License Information: Please see the attached LICENSE.TXT file
//
//////////////////////////////////////////////////////////////////////

#ifndef REFLECTION_SHADER_OP_
#define REFLECTION_SHADER_OP_

#include "../Interfaces/IShaderOp.h"
#include "../Utilities/Reference.h"

namespace RISE
{
	namespace Implementation
	{
		class ReflectionShaderOp : 
			public virtual IShaderOp, 
			public virtual Reference
		{
		protected:
			virtual ~ReflectionShaderOp();

		public:
			ReflectionShaderOp( 
				);

			//! Tells the shader to apply shade to the given intersection point
			void PerformOperation( 
				const RuntimeContext& rc,					///< [in] Runtime context
				const RayIntersection& ri,					///< [in] Intersection information 
				const IRayCaster& caster,					///< [in] The Ray Caster to use for all ray casting needs
				const IRayCaster::RAY_STATE& rs,			///< [in] Current ray state
				RISEPel& c,									///< [in/out] Resultant color from op
				const IORStack* const ior_stack,			///< [in/out] Index of refraction stack
				const ScatteredRayContainer* pScat			///< [in] Scattering information
				) const;

			//! Tells the shader to apply shade to the given intersection point for the given wavelength
			/// \return Amplitude of spectral function 
			Scalar PerformOperationNM( 
				const RuntimeContext& rc,					///< [in] Runtime context
				const RayIntersection& ri,					///< [in] Intersection information 
				const IRayCaster& caster,					///< [in] The Ray Caster to use for all ray casting needs
				const IRayCaster::RAY_STATE& rs,			///< [in] Current ray state
				const Scalar caccum,						///< [in] Current value for wavelength
				const Scalar nm,							///< [in] Wavelength to shade
				const IORStack* const ior_stack,			///< [in/out] Index of refraction stack
				const ScatteredRayContainer* pScat			///< [in] Scattering information
				) const;

			//! Asks if the shader op needs SPF data
			bool RequireSPF() const { return true; };
		};
	}
}

#endif
