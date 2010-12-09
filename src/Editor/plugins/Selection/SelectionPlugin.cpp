/************************************************************************
*
* vapor3D Editor � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "PCH.h"
#include "SelectionPlugin.h"
#include "SelectionManager.h"

#include "Editor.h"
#include "EditorIcons.h"
#include "Events.h"
#include "UndoManager.h"
#include "Viewframe.h"

namespace vapor { namespace editor {

//-----------------------------------//

namespace SelectionTool
{
	enum Enum
	{
		Select = wxID_HIGHEST + 42
	};
}

//-----------------------------------//

SelectionPlugin::SelectionPlugin( EditorFrame* frame )
	: Plugin(frame)
	, selections(nullptr)
	, buttonSelect(nullptr)
	, dragRectangle(nullptr)
{ }

//-----------------------------------//

PluginMetadata SelectionPlugin::getMetadata()
{
	PluginMetadata metadata;
	
	metadata.name = "Selection";
	metadata.description = "Provides selection services";
	metadata.author = "triton";
	metadata.version = "1.0";

	return metadata;
}

//-----------------------------------//

void SelectionPlugin::onPluginEnable()
{
	wxToolBar* toolbar = editor->getToolbar();

	wxBitmap iconSelect = wxMEMORY_BITMAP(cursor);
	buttonSelect = toolbar->AddTool( SelectionTool::Select, "Select",
		iconSelect, "Selects the Entity Selection tool", wxITEM_RADIO );
	addTool(buttonSelect);

	#pragma TODO("Initialize plugins events properly")
	Events* events = editor->getEventManager();
	events->currentPlugin = this;

	selections = new SelectionManager(editor);
}

//-----------------------------------//

void SelectionPlugin::onPluginDisable()
{
	delete selections;
	selections = nullptr;
}

//-----------------------------------//

void SelectionPlugin::onSceneLoad( const ScenePtr& scene )
{
	SelectionOperation* selection = selections->getSelection();

	if( selection )
		selection->unselectAll();
}

//-----------------------------------//

void SelectionPlugin::onMouseDrag( const MouseDragEvent& event )
{
	if( !dragRectangle )
	{
		dragOrigin = Vector2i(event.x, event.y );
		createRectangle();
	}

	updateRectangle( event );
}

//-----------------------------------//

void SelectionPlugin::createRectangle()
{
	//VertexBufferPtr vb = new VertexBuffer();
	//MaterialPtr mat = new Material("Drag");
	//RenderablePtr rend = new Renderable(PolygonType::Quads, vb, mat);

	//GeometryPtr geom = new Geometry();
	//geom->addRenderable( rend, RenderStage::Overlays );
	
	//Vector2i position = pos;
	//position.y -= view->getSize().y;

	OverlayPtr overlay( new Overlay() );
	overlay->setPositionMode( PositionMode::Absolute );

	dragRectangle.reset( new Entity() );
	dragRectangle->addTransform();
	dragRectangle->addComponent(overlay);
	editor->getEditorScene()->add(dragRectangle);
}

//-----------------------------------//

void SelectionPlugin::updateRectangle( const MouseDragEvent& event )
{
	RenderView* view = editor->getMainViewframe()->getView();
	OverlayPtr overlay = dragRectangle->getComponent<Overlay>();

	Vector2i dragPoint = Vector2i(event.x, event.y);
	
	if(dragPoint < dragOrigin)
	{
		dragOrigin = dragPoint;
		dragOrigin.y = view->getSize().y - dragOrigin.y;
	}

	Vector2i size = dragPoint - dragOrigin;
	size.x = abs(size.x);
	size.y = abs(size.y);
	
	Log::debug("Drag Coord: %d %d", event.x, event.y);
	Log::debug("Overlay Origin: %d %d", dragOrigin.x, dragOrigin.y);
	Log::debug("Overlay Size: %d %d", size.x, size.y);

	overlay->setPosition(dragOrigin);
	overlay->setSize(size);

	overlay->setPosition(0, 0);
	overlay->setSize( Vector2i(-100, -100) );

	//const RenderablePtr& rend =ja dragRectangle->getComponent<Geometry>();
	//const RenderablePtr& rend = dragRectangle->getRenderable();
	//const VertexBufferPtr& vb = rend->getVertexBuffer();
}

//-----------------------------------//

void SelectionPlugin::onMouseButtonPress( const MouseButtonEvent& event )
{
	dragOrigin.x = event.x;
	dragOrigin.y = event.y;
}

//-----------------------------------//

void SelectionPlugin::onMouseButtonRelease( const MouseButtonEvent& event )
{
	if(dragRectangle)
	{
		editor->getEditorScene()->remove(dragRectangle);
		dragRectangle.reset();
	}

	if( event.button != MouseButton::Left )
		return;
	
	SelectionOperation* selection = nullptr;
	SelectionOperation* selected = selections->getSelection();

	EntityPtr entity;
	if( !getPickEntity(event.x, event.y, entity) )
	{
		if( !selected )
			return;

		// If there is a current selection, and the user pressed the mouse,
		// then we need to unselect everything that is currently selected.

		selection = selections->createOperation();
		selection->previous = selected->selections;
		selection->mode = SelectionMode::None;
	}
	else
	{
		if(selections->getSelectionMode() != SelectionMode::Entity)
			return;

		if( selected )
			selected->unselectAll();

		selection = selections->createOperation();

		if( selected )
			selection->previous = selected->selections;

		selection->addEntity(entity);
	}

	assert( selection != nullptr );

	UndoManager* undoManager = editor->getUndoManager();
	undoManager->registerOperation(selection);

	selection->redo();
}

//-----------------------------------//

bool SelectionPlugin::getPickEntity(int x, int y, EntityPtr& entity)
{
	Viewframe* viewframe = editor->getMainViewframe();
	RenderView* view = viewframe->getView();
	
	const CameraPtr& camera = view->getCamera();
	const ScenePtr& scene = engine->getSceneManager();

	// Get a ray given the screen location clicked.
	Vector3 outFar;
	const Ray& pickRay = camera->getRay( x, y, &outFar );

#if 0 // Enable this to draw debugging lines
	const EntityPtr& line = buildRay( pickRay, outFar );
	editor->getEditorScene()->add( line );
#endif

	// Perform ray casting to find the nodes.
	RayBoxQueryResult res;
	if( !scene->doRayBoxQuery(pickRay, res) )
		return false;

	entity = res.node;
	return true;
}

//-----------------------------------//

SelectionManager* SelectionPlugin::getSelectionManager() const
{
	return selections;
}

//-----------------------------------//

} } // end namespaces