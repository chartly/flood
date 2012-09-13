/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "Engine/API.h"
#include "Engine/Resources/BitmapFont.h"

#ifdef ENABLE_FONT_BITMAP

NAMESPACE_ENGINE_BEGIN

//-----------------------------------//

const Vector2 BitmapFont::getGlyphSize() const
{
	// TODO: ...
	return Vector2(16, 16);
}

//-----------------------------------//

NAMESPACE_ENGINE_END

#endif