/************************************************************************
*
* vapor3D Engine © (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "vapor/PCH.h"
#include "vapor/scene/Skydome.h"
#include "vapor/math/Math.h"

namespace vapor { namespace scene {

using namespace vapor::math;
using namespace vapor::render;

//-----------------------------------//

const std::string& Skydome::type = "Skydome";

//-----------------------------------//

Skydome::Skydome( const render::MaterialPtr& mat )
	: dome( new Sphere( false, 2 ) )
{
	MaterialPtr matSky( new Material( "SkydomeSimple" ) );
	
	//matSky->setDepthWrite( false );
	matSky->setBackfaceCulling( false );
	matSky->setProgram( "diffuse" );

	dome->setPolygonMode( PolygonMode::Solid );
	dome->setMaterial( matSky );

	//setSkyLinearGradient( Colors::SkyBlue, Colors::White );
	setSkyLinearGradient( Color( 160, 50, 0 ), Colors::White );

	addRenderable( dome );
}

//-----------------------------------//

Skydome::~Skydome()
{

}

//-----------------------------------//

const std::string& Skydome::getType() const
{
	return type;
}

//-----------------------------------//

void Skydome::setSkyColor( const math::Color& color )
{
	VertexBufferPtr vb = dome->getVertexBuffer();
	uint numVertices = vb->getNumVertices();

	std::vector<Vector3> colors;
	colors.resize( numVertices, Vector3( color.r, color.g, color.b ) );

	vb->set( VertexAttribute::Color, colors );
}

//-----------------------------------//

static float rMin = 0.0f;
static float rMax = 1.0f;

float Skydome::scale( float number )
{
	number += abs(yMin);
	return (number / ((yMax) / (rMax - rMin))) + rMin;
}

void Skydome::setSkyLinearGradient( const math::Color& c1, const math::Color& c2 )
{
	colorTop = Vector3( c1.r, c1.g, c1.b );
	colorBottom = Vector3( c2.r, c2.g, c2.b );

	VertexBufferPtr vb = dome->getVertexBuffer();
	uint numVertices = vb->getNumVertices();

	std::vector<Vector3> colors;

	const std::vector<math::Vector3>& vertices = vb->getVertices();

	// Search for the max and min Y coordinate.
	yMin = std::numeric_limits<float>::max();
	yMax = std::numeric_limits<float>::min();

	foreach( const Vector3& vec, vertices )
	{
		if( vec.y > yMax ) 
			yMax = vec.y;
		else if( vec.y < yMin ) 
			yMin = vec.y;
	}

	yMax += abs(yMin);
	//yMin += abs(yMin);

	foreach( const Vector3& vec, vertices )
	{
		Color c = getSkyVertexColor( vec );
		colors.push_back( Vector3( c.r, c.g, c.b ) );
	}	

	vb->set( VertexAttribute::Color, colors );
}

//-----------------------------------//

Color Skydome::getSkyVertexColor( const Vector3& v )
{
	// Convert cartersian coordinates to spherical coordinates.
	// See: http://mathworld.wolfram.com/SphericalCoordinates.html

	float r = math::sqrt( pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2) );

	// Varies from 0 to 2pi
	float t = acos( v.y / r );
	
	// Varies from 0 to pi
	//float p = atan( v.y / math::sqrt( pow(v.x, 2) + pow(v.y, 2) ) );

	float lat = math::asinf(v.z / r);
	float lon = math::atan2(v.y, v.x);

	// Linear interpolate to get the color at the vertex.
	Vector3 newColor = colorBottom + (colorTop - colorBottom) * scale( v.y );

	debug( "(%f,%f,%f) %f %f", v.x, v.y, v.z, scale( v.y ), r);
	
	return Color( newColor.x, newColor.y, newColor.z );
}

//-----------------------------------//

} } // end namespaces
