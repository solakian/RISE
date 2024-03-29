//////////////////////////////////////////////////////////////////////
//
//  AshikminShirleyAnisotropicPhongSPF.h - Defines the SPF component
//  of the Shirley / Ashikhmin Anisotropic Phong BRDF model
//
//  Author: Aravind Krishnaswamy
//  Date of Birth: May 21, 2003
//  Tabs: 4
//  Comments:  
//
//  License Information: Please see the attached LICENSE.TXT file
//
//////////////////////////////////////////////////////////////////////

#ifndef ASHIKMINSHIRLEY_ANISOTROPIC_PHONG_SPF_
#define ASHIKMINSHIRLEY_ANISOTROPIC_PHONG_SPF_

#include "../Interfaces/ISPF.h"
#include "../Interfaces/IPainter.h"
#include "../Utilities/Reference.h"

namespace RISE
{
	namespace Implementation
	{
		class AshikminShirleyAnisotropicPhongSPF : public virtual ISPF, public virtual Reference
		{
		protected:
			virtual ~AshikminShirleyAnisotropicPhongSPF( );

			const IPainter&			Nu;				// Phong exponents
			const IPainter&			Nv;

			const IPainter&			Rd;				// diffuse reflectance
			const IPainter&			Rs;				// specular reflectance

		public:
			AshikminShirleyAnisotropicPhongSPF( const IPainter& Nu_, const IPainter& Nv_, const IPainter& Rd_, const IPainter& Rs_ );

			//! Given parameters describing the intersection of a ray with a surface, this will return
			//! the reflected and transmitted rays along with attenuation factors.  
			void	Scatter( 
				const RayIntersectionGeometric& ri,							///< [in] Geometric intersection details for point of intersection
				const RandomNumberGenerator& random,				///< [in] Random number generator
				ScatteredRayContainer& scattered,							///< [out] The list of scattered rays from the surface
				const IORStack* const ior_stack								///< [in/out] Index of refraction stack
				) const;

			//! Given parameters describing the intersection of a ray with a surface, this will return
			//! the reflected and transmitted rays along with attenuation factors which taking into 
			//! account spectral affects.  
			void	ScatterNM( 
				const RayIntersectionGeometric& ri,							///< [in] Geometric intersection details for point of intersection
				const RandomNumberGenerator& random,				///< [in] Random number generator
				const Scalar nm,											///< [in] Wavelength the material is to consider (only used for spectral processing)
				ScatteredRayContainer& scattered,							///< [out] The list of scattered rays from the surface
				const IORStack* const ior_stack								///< [in/out] Index of refraction stack
				) const;
		};
	}
}

#endif

