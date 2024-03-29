//////////////////////////////////////////////////////////////////////
//
//  IrradianceCache.cpp - Implementation of the irradiance cache
//
//  Author: Aravind Krishnaswamy
//  Date of Birth: May 28, 2002
//  Tabs: 4
//  Comments:  
//
//  License Information: Please see the attached LICENSE.TXT file
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "IrradianceCache.h"

using namespace RISE;
using namespace RISE::Implementation;

/////////////////////////////////////////////////////////////////
// CacheElement implementations
/////////////////////////////////////////////////////////////////

IrradianceCache::CacheElement::CacheElement( 
	const Point3& pos, 
	const Vector3& norm, 
	const RISEPel& rad, 
	const Scalar r0_, 
	const Scalar weight, 
	const RISEPel* rot, 
	const RISEPel* tran 
	) : 
  ptPosition( pos ),
  vNormal( norm ),
  cIRad( rad ),
  r0( r0_ ),
  dWeight( weight )
{
	if( rot ) {
		rotationalGradient[0] = rot[0];
		rotationalGradient[1] = rot[1];
		rotationalGradient[2] = rot[2];
	}

	if( tran ) {
		translationalGradient[0] = tran[0];
		translationalGradient[1] = tran[1];
		translationalGradient[2] = tran[2];
	}
}

IrradianceCache::CacheElement::CacheElement( const CacheElement& ce, const Scalar weight ) : 
  ptPosition( ce.ptPosition ),
  vNormal( ce.vNormal ),
  cIRad( ce.cIRad ),
  r0( ce.r0 ),
  dWeight( weight )
{
}

IrradianceCache::CacheElement::~CacheElement( )
{

}

Scalar IrradianceCache::CacheElement::ComputeWeight( const Point3& pos, const Vector3& norm ) const
{
	const Scalar denom = Point3Ops::Distance(pos, ptPosition)/r0 + sqrt( 1.0 - r_min(1.0,Vector3Ops::Dot( norm, vNormal )) );
	return 1.0/denom;
}

/////////////////////////////////////////////////////////////////
// CacheNode implementations
/////////////////////////////////////////////////////////////////

IrradianceCache::CacheNode::CacheNode( const Scalar size, const Point3& center ) : 
  ptCenter( center ), dSize( size )
{
	for( int i=0; i<8; i++ ) {
		pChildren[i] = 0;
	}
}

IrradianceCache::CacheNode::~CacheNode( )
{
	Clear();
}

void IrradianceCache::CacheNode::Clear()
{
	for( int i=0; i<8; i++ ) {
		if( pChildren[i] ) {
			GlobalLog()->PrintDelete( pChildren[i], __FILE__, __LINE__ );
			delete pChildren[i];
			pChildren[i] = 0;
		}
	}
}

unsigned char IrradianceCache::CacheNode::WhichNode( const Point3& pos )
{
	//
	// First check left or right
	//

	// On the right
	if( pos.x > ptCenter.x )
	{
		// Check top and bottom
		if( pos.y > ptCenter.y )
		{
			// On the top
			
			// Check front and back
			if( pos.z > ptCenter.z ) {
				// in the back
				return 0;
			} else {
				return 1;
			}
		}
		else
		{
			// On the bottom

			if( pos.z > ptCenter.z ) {
				// Back
				return 2;
			} else {
				return 3;
			}
		}
	}
	else
	{
		// Check top and bottom
		if( pos.y > ptCenter.y )
		{
			// On the top
			
			// Check front and back
			if( pos.z > ptCenter.z ) {
				// in the back
				return 4;
			} else {
				return 5;
			}
		}
		else
		{
			// On the bottom

			if( pos.z > ptCenter.z ) {
				// Back
				return 6;
			} else {
				return 7;
			}
		}
	}
}

void IrradianceCache::CacheNode::InsertElement(
	const CacheElement& elem, 
	const Scalar tolerance 
	)
{
	//
	// If the radiant energy is big enough for this node, then insert, otherwise
	// recurse down to the children and see if they can handle it
	//
	if( dSize < 4.0*elem.r0*tolerance ) {
		values.push_back( elem );
	} else {
		// Create children if we have to
		// First find out which child this value will be inserted into
		unsigned char idx = WhichNode( elem.ptPosition );
		Scalar	dChildSize = dSize * 0.5;

		if( pChildren[idx] == 0 )
		{
			switch( idx )
			{
			case 0:
				pChildren[idx] = new CacheNode( dChildSize, Point3Ops::mkPoint3( ptCenter, Vector3( dChildSize, dChildSize, dChildSize ) ) );
				break;
			case 1:
				pChildren[idx] = new CacheNode( dChildSize, Point3Ops::mkPoint3( ptCenter, Vector3( dChildSize, dChildSize, -dChildSize ) ) );
				break;
			case 2:
				pChildren[idx] = new CacheNode( dChildSize, Point3Ops::mkPoint3( ptCenter, Vector3( dChildSize, -dChildSize, dChildSize ) ) );
				break;
			case 3:
				pChildren[idx] = new CacheNode( dChildSize, Point3Ops::mkPoint3( ptCenter, Vector3( dChildSize, -dChildSize, -dChildSize ) ) );
				break;
			case 4:
				pChildren[idx] = new CacheNode( dChildSize, Point3Ops::mkPoint3( ptCenter, Vector3( -dChildSize, dChildSize, dChildSize ) ) );
				break;
			case 5:
				pChildren[idx] = new CacheNode( dChildSize, Point3Ops::mkPoint3( ptCenter, Vector3( -dChildSize, dChildSize, -dChildSize ) ) );
				break;
			case 6:
				pChildren[idx] = new CacheNode( dChildSize, Point3Ops::mkPoint3( ptCenter, Vector3( -dChildSize, -dChildSize, dChildSize ) ) );
				break;
			case 7:
				pChildren[idx] = new CacheNode( dChildSize, Point3Ops::mkPoint3( ptCenter, Vector3( -dChildSize, -dChildSize, -dChildSize ) ) );
				break;
			}

			GlobalLog()->PrintNew( pChildren[idx], __FILE__, __LINE__, "child node" );
		}

		pChildren[idx]->InsertElement( elem, tolerance );
	}
}

Scalar IrradianceCache::CacheNode::Query( 
	const Point3& ptPosition, 
	const Vector3& vNormal, 
	std::vector<CacheElement>& results, 
	const Scalar invTolerance
	) const
{
	std::vector<CacheElement>::const_iterator		i, e;
	Scalar		accruedWeights = 0;

	for( i=values.begin(), e=values.end(); i!=e; i++ )
	{
		const CacheElement& elem = *i;
		Scalar	thisWeight = r_min( 1e10, elem.ComputeWeight( ptPosition, vNormal ) );

		if( thisWeight > invTolerance ) {
			CacheElement newelem( elem, thisWeight );
			results.push_back( newelem );
			accruedWeights += thisWeight;
		}
	}

	for( int x = 0; x<8; x++ )
	{
		if( (pChildren[x]) &&
			(pChildren[x]->ptCenter.x - dSize <= ptPosition.x && ptPosition.x <= pChildren[x]->ptCenter.x + dSize ) &&
			(pChildren[x]->ptCenter.y - dSize <= ptPosition.y && ptPosition.y <= pChildren[x]->ptCenter.y + dSize ) &&
			(pChildren[x]->ptCenter.z - dSize <= ptPosition.z && ptPosition.z <= pChildren[x]->ptCenter.z + dSize ) )
		{
			accruedWeights += pChildren[x]->Query( ptPosition, vNormal, results, invTolerance );
		}

	}

	return accruedWeights;
}

bool IrradianceCache::CacheNode::IsSampleNeeded(
	const Point3& ptPosition, 
	const Vector3& vNormal, 
	const Scalar invTolerance
	) const
{
	std::vector<CacheElement>::const_iterator		i, e;

	// Check the values at this node first and see if any will do, if they suffice, we're ok and we say no
	for( i=values.begin(), e=values.end(); i!=e; i++ ) {
		const CacheElement& elem = *i;
		Scalar	thisWeight = r_min( 1e10, elem.ComputeWeight( ptPosition, vNormal ) );

		if( thisWeight > invTolerance ) {
			return false;
		}
	}

	// Otherwise check children
	for( int x = 0; x<8; x++ )
	{
		if( (pChildren[x]) &&
			(pChildren[x]->ptCenter.x - dSize <= ptPosition.x && ptPosition.x <= pChildren[x]->ptCenter.x + dSize ) &&
			(pChildren[x]->ptCenter.y - dSize <= ptPosition.y && ptPosition.y <= pChildren[x]->ptCenter.y + dSize ) &&
			(pChildren[x]->ptCenter.z - dSize <= ptPosition.z && ptPosition.z <= pChildren[x]->ptCenter.z + dSize ) )
		{
			bool bChildSampleCheck = pChildren[x]->IsSampleNeeded( ptPosition, vNormal, invTolerance );
			if( !bChildSampleCheck ) {
				return false;
			}
		}

	}

	return true;
}

/////////////////////////////////////////////////////////////////
// IrradianceCache implementations
/////////////////////////////////////////////////////////////////

IrradianceCache::IrradianceCache( 
	const Scalar size, 
	const Scalar tol, 
	const Scalar min
	) : 
  root( CacheNode( size, Point3( 0, 0, 0 ) ) ),
  tolerance( tol ), 
  invTolerance( 1.0/tol ),
  min_spacing( min ), 
  bPreComputed( false )
{
	max_spacing = min_spacing * 100.0;
}

IrradianceCache::~IrradianceCache( )
{
}

