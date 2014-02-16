/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "Core/Math/Vector.h"
#include "Core/Math/EulerAngles.h"
#include "Core/Math/Matrix4x3.h"
#include "Core/RefPtr.h"

FWD_DECL_INTRUSIVE(Bone)
FWD_DECL_INTRUSIVE(Animation)

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

struct API_ENGINE KeyFrame
{
	// Time of the frame.
	float time;

	// Position of the frame.
	Vector3 position;

	// Rotation of the frame.
	EulerAngles rotation;
};

//-----------------------------------//

/**
 * An animation is composed of key frames, a collection of discrete
 * values for a given parameter, usually position and rotation, that
 * will be interpolated at a given speed rate to produce animation.
 */

class API_ENGINE Animation : public ReferenceCounted
{
public:

	Animation();

	// Gets if the animation is looped.
	bool isLooped();

	// Gets the total animation time.
	float getTotalTime() const;

	// Gets the interpolated key frame matrix.
	Matrix4x3 getKeyFrameMatrix(const BonePtr& bone, float time);

	// Name of this animation.
	String name;

	// Loop state of this animation.
	bool looped;

    // Key frames of this animation.
    Array<KeyFrame> keyFramesVector;

	// Key frames of this animation per bone.
    HashMap<Array<KeyFrame>> keyFrames;
};

//-----------------------------------//

struct API_ENGINE AnimationState
{
	// Animation data.
	AnimationPtr animation;

	// Animation time.
	float animationTime;

	// Interpolated bones matrices.
	Array<Matrix4x3> bonesMatrix;
};

//-----------------------------------//

struct API_ENGINE AnimationMetadata
{
	int start;
	int end;
	String name;
	float startTime;
};

//-----------------------------------//

NAMESPACE_ENGINE_END
