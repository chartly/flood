/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "Engine/API.h"

#include "Engine/Scene/Skydome.h"
#include "Engine/Scene/Camera.h"
#include "Engine/Scene/Entity.h"

#include "Core/Math/Noise.h"
#include "Core/Math/Helpers.h"

#include "Graphics/RenderDevice.h"
#include "Graphics/RenderView.h"

#include "Resources/ResourceManager.h"

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

REFLECT_CHILD_CLASS(Skydome, Geometry)
	FIELD_PRIMITIVE_SETTER(4, Color, skyBaseColor, SkyColorBase)
	FIELD_PRIMITIVE_SETTER(5, Color, skyExtraColor, SkyColorExtra)
	FIELD_PRIMITIVE_SETTER(6, bool, useLinearGradient, SkyLinearGradient)
	FIELD_PRIMITIVE_SETTER(7, bool, showClouds, CloudsVisible)
	FIELD_PRIMITIVE(8, float, cloudsCover)
	FIELD_PRIMITIVE(9, float, cloudsSharpness)
	FIELD_PRIMITIVE(10, int32, cloudsNoiseSeed)
	//FIELD_PRIMITIVE(11, bool, followCamera)
	//FIELD_CLASS_PTR(Image, clouds)
REFLECT_CLASS_END()

//-----------------------------------//

Skydome::Skydome()
	: currentTime(0)
	, followCamera(true)
	, skyBaseColor(Color::SkyBlue)
	, skyExtraColor(Color::Black)
	, needsRegeneration(true)
	, cloudsCover(0.95f)
	, cloudsSharpness(0.95f)
	, cloudsNoiseSeed(1343654258)
{

}

//-----------------------------------//

void Skydome::setSunEntity( const EntityPtr& sunEntity )
{
	sun = sunEntity->getTransform();
}

//-----------------------------------//

void Skydome::setSkyColorBase( const Color& color )
{
	skyBaseColor = color;
	needsRegeneration = true;
}

//-----------------------------------//

void Skydome::setSkyColorExtra( const Color&  color)
{
	skyExtraColor = color;
	needsRegeneration = true;
}

//-----------------------------------//

void Skydome::setSkyLinearGradient( bool isLinearGradient )
{
	useLinearGradient = isLinearGradient;
	needsRegeneration = true;
}

//-----------------------------------//

void Skydome::setCloudsVisible( bool clouds )
{
	showClouds = clouds;
}

//-----------------------------------//

static float FloatExpCurve(float v, float cover, float sharpness)
{
	float c = std::max<float>(v - (1 - cover), 0);
	return 1 - pow(1 - sharpness, c);
}

//-----------------------------------//

float FloatScale( float number, float minY, float maxY )
{
	const float rMin = 0.0f;
	const float rMax = 1.0f;

	number += abs(minY);
	return (number / ((maxY) / (rMax - rMin))) + rMin;
}

//-----------------------------------//

void Skydome::generateClouds()
{
	if( !clouds ) return;

	Array<byte> bn;
	bn.reserve( cloudsNoise.size()*4 );

	for( size_t i = 0; i < cloudsNoise.size(); i++ )
	{
		const float& n = cloudsNoise[i];
		bn.pushBack( 0 );
		bn.pushBack( 0 );
		bn.pushBack( 0 );
		bn.pushBack( FloatExpCurve(n, cloudsCover, cloudsSharpness)*255 );
	}

	Image* image = clouds.Resolve();
	image->setBuffer(bn);

	MaterialHandle materialHandle = dome->getMaterial();
	
	Material* material = materialHandle.Resolve();
	material->setTexture(0, clouds);
}

//-----------------------------------//

const float DEFAULT_SPHERE_RADIUS = 5000.0f;

void Skydome::generateDome()
{
	MaterialHandle materialHandle = MaterialCreate( AllocatorGetHeap(), "Sky");
	
	Material* material = materialHandle.Resolve();
	material->setShader( "Sky" );

	dome = AllocateHeap(Sphere, false, 2, DEFAULT_SPHERE_RADIUS);
	dome->setPrimitiveRasterMode( PrimitiveRasterMode::Solid );
	dome->setMaterial( materialHandle );
	
	addRenderable( dome );
}

//-----------------------------------//

void Skydome::generateBodies()
{
	static const uint16 CLOUDS_WIDTH = 256;
	static const uint16 CLOUDS_HEIGHT = 256;

	cloudsNoise.clear();

	// Generate clouds noise.
	Noise noise(cloudsNoiseSeed);
	noise.generate(cloudsNoise, CLOUDS_WIDTH, CLOUDS_HEIGHT);

	clouds = ImageCreate(AllocatorGetHeap(), CLOUDS_WIDTH, CLOUDS_HEIGHT, PixelFormat::R8G8B8A8);
	generateClouds();
}

//-----------------------------------//

void Skydome::generateColors()
{
	GeometryBufferPtr gb = dome->getGeometryBuffer();
	uint32 numVertices = gb->getNumVertices();

	Array<Vector3> colors;

	if(!useLinearGradient)
	{
		colors.resize( numVertices );
		std::fill(colors.begin(), colors.end(), skyBaseColor);
	}
	else
	{
		// Search for the max and min Y coordinate.
		float minY = LimitsFloatMaximum;
		float maxY = LimitsFloatMinimum;

		for( size_t i = 0; i < numVertices; i++ )
		{
			Vector3* vec = (Vector3*) gb->getAttribute(VertexAttribute::Position, i);

			minY = std::min(vec->y, minY);
			maxY = std::max(vec->y, maxY);
		}

		maxY += abs(minY);
		//minY += abs(minY);

		for( size_t i = 0; i < numVertices; i++ )
		{
			Vector3* vec = (Vector3*) gb->getAttribute(VertexAttribute::Position, i);

			const Color& color = getSkyVertexColor( *vec, minY, maxY );
			colors.pushBack( color );
		}	
	}

	gb->set( VertexAttribute::Color, colors );
	gb->forceRebuild();
}

//-----------------------------------//

Color Skydome::getSkyVertexColor( const Vector3& v, float minY, float maxY )
{
	float s = FloatScale( v.y, minY, maxY );
	float t = (s <= 0.5f) ? 0.0f : s - 0.5f;

	// Linear interpolate to get the color at the vertex.
	Vector3 newColor = ((Vector3) skyBaseColor).lerp(skyExtraColor, t);

	return Color( newColor.x, newColor.y, newColor.z );
}

//-----------------------------------//

void Skydome::update( float delta )
{
	if( renderables.empty() )
	{
		generateDome();
		generateBodies();
	}

	if(needsRegeneration)
	{
		generateColors();
		needsRegeneration = false;
	}

	// Updates the current game time.
	#pragma TODO("Create a game time subsystem")

	currentTime += delta;
	
	if(!dome) return;

	// Update the renderable uniforms with the updated scroll rate.
	for( size_t i = 0; i < renderables.size(); ++i )
	{
		Renderable* rend = renderables[i].get();
		UniformBuffer* ub = rend->getUniformBuffer().get();
		ub->setUniform("vp_TexScroll", float(currentTime / 100.0f));
	}

#if 0
	if( followCamera )
	{
		CameraPtr camera = getCurrentCamera();

		// Gets the current camera position.
		TransformPtr transCamera = camera->getEntity()->getTransform();
		Vector3 cameraPosition = transCamera->getPosition();

		// Sets the sky position to the camera position.
		EntityPtr nodeSky = getEntity();
		TransformPtr transSky = nodeSky->getTransform();
		transSky->setPosition( cameraPosition );
	}
#endif
}

//-----------------------------------//

Vector3 Skydome::getSunPosition()
{
	// We'll use spherical coordinates to get the new position for the
	// sun. These are pretty simple to use and adapt pretty well to 
	// what we want to achieve here.

	float rho = DEFAULT_SPHERE_RADIUS * 0.95f;
	float phi = PI * currentTime * 0.01f;
	float theta = 0;

	// We want 12 hours of virtual time to correspond to the day cycle,
	// so the sun should start at half-sphere when time reaches a given
	// sunrise time (say 6am), and go on until the sunset time.

	float x = rho*sin(phi)*cos(theta);
	float y = rho*sin(phi)*sin(theta);
	float z = rho*cos(phi);

	return Vector3(x, y, z);
}

//-----------------------------------//

NAMESPACE_ENGINE_END