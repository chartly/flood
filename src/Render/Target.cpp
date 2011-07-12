/************************************************************************
*
* vapor3D Engine � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "Engine/API.h"
#include "Render/Target.h"
#include "Render/View.h"

namespace vapor {

//-----------------------------------//

RenderTarget::RenderTarget()
{ }

//-----------------------------------//

RenderTarget::~RenderTarget()
{
	for( size_t i = 0; i < views.size(); i++ )
	{
		RenderView* view = views[i];
		Deallocate(view);
	}
}

//-----------------------------------//

RenderView* RenderTarget::createView()
{
	RenderView* view = Allocate(RenderView, AllocatorGetHeap());
	view->setRenderTarget(this);

	views.push_back( view );
	return view;
}

//-----------------------------------//

Vector2i Settings::getSize() const
{
	return Vector2i(width, height);
}

//-----------------------------------//

} // end namespace