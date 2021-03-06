/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#pragma once

#include "Core/Stream.h"
#include "Engine/Resources/Mesh.h"
#include "Engine/Resources/Animation.h"
#include "Pipeline/MilkshapeFormat.h"

NAMESPACE_RESOURCES_BEGIN

//-----------------------------------//

struct ms3d_vertex_t;
struct ms3d_triangle_t;
struct ms3d_group_t;
struct ms3d_material_t;
struct ms3d_joint_t;
struct ms3d_keyframe_t;

/**
 * Represents Milkshape 3D meshes. These can hold vertex, animation, and 
 * material data, are relatively easy to parse and use, and documented.
 */

class Milkshape3D
{
public:

	Milkshape3D();

	// Loads the mesh data into the resource.
	bool load(const Stream& stream);

	// Preprocesses the mesh.
	void preprocess();

	// Builds the mesh geometry data.
	void buildGeometry();

	// Builds the mesh skeleton data.
	void buildSkeleton();

	// Builds the mesh animation data.
	void buildAnimations();

	// Builds an animation from a metadata.
	Animation* buildAnimation(AnimationMetadata& data);

	// Builds the animation metadata.
	void buildAnimationMetadata();

	// Builds the key frame data for a joint.
	void buildKeyFrames( const ms3d_joint_t& joint,
		const AnimationMetadata& data, KeyFramesVector& frames );

	// Sets the mesh.
	SETTER(Mesh, Mesh*, mesh)

protected:

	// Reads the meshes structures from the data.
	bool readHeader();
	void readVertices();
	void readTriangles();
	void readGroups();
	void readMaterials();
	void readAnimation();
	void readJoints();
	void readComments();

	// Builds the material for a group.
	MeshMaterial buildMaterial( const ms3d_group_t& group );

	// Gets the start of the animation.
	float getAnimationStart(const AnimationMetadata& data);

	// Returns if a group has a material.
	bool hasMaterial( const ms3d_group_t& group );

	// Sets up the joints hierarchy indices.
	void setupJointsHierarchy();

	// Sets up the joint matrices.
	void setupJointMatrices();

	// Sets up the joint rotations.
	void setupJointRotations();

	// Finds a joint by name.
	int findJoint(const char* name);

	// Contents of the file.
	Array<byte> filebuf;
	unsigned long index;

	// Mesh data structures.
	Array<ms3d_vertex_t*> vertices;
	Array<ms3d_triangle_t*> triangles;
	Array<ms3d_group_t> groups;
	Array<ms3d_material_t*> materials;
	Array<ms3d_joint_t> joints;
	String mainComment;

	// Animation metadata.
	Array<AnimationMetadata> metadata;

	// Animation FPS.
	float animationFPS;

	// Total frames of animation.
	int totalFrames;

	// Mesh.
	Mesh* mesh;
};

TYPEDEF_RESOURCE_HANDLE_FROM_TYPE( Milkshape3D );

//-----------------------------------//

NAMESPACE_RESOURCES_END