/************************************************************************
*
* vapor3D Editor � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "Editor/API.h"
#include "SceneDocument.h"
#include "Editor.h"
#include "Events.h"
#include "RenderControl.h"
#include "EditorIcons.h"
#include "ResourceDrop.h"
#include "Core/Utilities.h"
#include "Plugin.h"
#include "PluginManager.h"
#include "../Selection/SelectionPlugin.h"

namespace vapor { namespace editor {

//-----------------------------------//

SceneDocument::SceneDocument()
	: viewFrame(nullptr)
	, toolbar(nullptr)
{
	createView();
	createScene();

	RenderControl* control = viewFrame->getControl();
	control->Bind(wxEVT_RIGHT_UP, &SceneDocument::OnMouseRightUp, this); 
	control->Bind(wxEVT_RIGHT_DOWN, &SceneDocument::OnMouseRightDown, this);
	control->Bind(wxEVT_LEFT_DOWN, &SceneDocument::OnMouseEvent, this);
	control->Bind(wxEVT_RIGHT_DOWN, &SceneDocument::OnMouseEvent, this);
	control->Bind(wxEVT_MOUSEWHEEL, &SceneDocument::OnMouseEvent, this);

	ResourceManager* res = GetResourceManager();
	res->loadQueuedResources();

	// Update at least once before rendering.
	onUpdate(0);

	//events->onSceneLoad(scene);
}

//-----------------------------------//

SceneDocument::~SceneDocument()
{
	if(!scene) return;

	Events* events = GetEditor().getEventManager();
	events->onSceneUnload(scene);
}

//-----------------------------------//

void SceneDocument::OnMouseRightUp(wxMouseEvent& event)
{
	cameraController->setEnabled(false);
	getRenderWindow()->setCursorVisiblePriority(true, 100);

	event.Skip();
}

//-----------------------------------//

void SceneDocument::OnMouseRightDown(wxMouseEvent& event)
{
	cameraController->setEnabled(true);
	getRenderWindow()->setCursorVisiblePriority(false, 100);

	//event.Skip();
}

//-----------------------------------//

wxWindow* SceneDocument::getWindow()
{
	return viewFrame;
}

//-----------------------------------//

void SceneDocument::setupRenderWindow()
{
	RenderDevice* renderDevice = GetEngine()->getRenderDevice();

	RenderControl* control = viewFrame->getControl();
	Window* window = (Window*) control->getRenderWindow();

	renderDevice->setWindow( window );
	renderDevice->setRenderTarget( window );

	renderDevice->init();
}

//-----------------------------------//

void SceneDocument::onDocumentSelect()
{
	setupRenderWindow();

	RenderControl* control = viewFrame->getControl();
	control->startFrameLoop();

	wxAuiManager* aui = GetEditor().getAUI();
	aui->GetPane("Hierarchy").Show();
	aui->GetPane("Properties").Show();
	aui->Update();

	GetEditor().getEventManager()->onSceneLoad(scene);
}

//-----------------------------------//

void SceneDocument::onDocumentUnselect()
{
	RenderControl* control = viewFrame->getControl();
	control->stopFrameLoop();

	wxAuiManager* aui = GetEditor().getAUI();
	aui->GetPane("Hierarchy").Hide();
	aui->GetPane("Properties").Hide();
	aui->Update();
}

//-----------------------------------//

void SceneDocument::OnMouseEvent(wxMouseEvent& event)
{
	getRenderControl()->SetFocus();
	event.Skip();
}

//-----------------------------------//

void SceneDocument::createView()
{
	viewFrame = new Viewframe( &GetEditor() );

	toolbar = createContextToolbar();
	toolbar->Realize();

	RenderControl* control = viewFrame->createControl();
	control->onRender.Bind( this, &SceneDocument::onRender );
	control->onUpdate.Bind( this, &SceneDocument::onUpdate );
	control->SetDropTarget( new ResourceDropTarget( &GetEditor() ) );
	control->SetFocus();
	setupRenderWindow();

	RenderView* view = viewFrame->createView();
	#pragma TODO(Use configurable colors)
	view->setClearColor( Color(0.0f, 0.10f, 0.25f) );

	viewFrame->mainSizer->Add(toolbar, wxSizerFlags().Expand().Top());
}

//-----------------------------------//

wxAuiToolBar* SceneDocument::createContextToolbar()
{
	wxAuiToolBar* tb = new wxAuiToolBar(viewFrame);
	return tb;
}

//-----------------------------------//

void SceneDocument::onToolSelect(PluginTool* mode)
{
	wxSizer* sizer = getViewframe()->mainSizer;
	wxSizerItem* item = sizer->GetChildren().GetLast()->GetData();
	
	wxWindow* currentWindow = item->GetWindow();
	currentWindow->Hide();

	wxAuiToolBar* newToolbar = (mode->toolbar) ? mode->toolbar : toolbar;
	newToolbar->Show();
	
	item->AssignWindow(newToolbar);
	sizer->Layout();
}

//-----------------------------------//

void SceneDocument::createScene()
{
	scene.reset( new Scene() );
	editorScene.reset( new Scene() );
	
	// Create a nice grid for the editor.
	EntityPtr nodeGrid( EntityCreate(AllocatorGetHeap()) );
	nodeGrid->setName("Grid");
	nodeGrid->addTransform();
	nodeGrid->addComponent( GridPtr( new Grid() ) );
	nodeGrid->setTag( Tags::NonPickable, true );
	editorScene->add( nodeGrid );

	Vector3 initialPosition(0, 20, -65);
	EntityPtr nodeCamera( createCamera() );
	nodeCamera->getTransform()->setPosition(initialPosition);
	editorScene->add( nodeCamera );

	CameraPtr camera = nodeCamera->getComponent<Camera>();
	viewFrame->setMainCamera(camera);
	viewFrame->switchToDefaultCamera();
}

//-----------------------------------//

EntityPtr SceneDocument::createCamera()
{
	// So each camera will have unique names.
	static uint8 i = 0;

	// Create a new first-person camera for our view.
	// By default it will be in perspective projection.
	CameraPtr camera( new Camera() );
	cameraController.reset( new FirstPersonController() );
	cameraController->setEnabled(false);

	Frustum& frustum = camera->getFrustum();
	frustum.farPlane = 10000;

	// Generate a new unique name.
	String name( "EditorCamera"+StringFromNumber(i++) );

	EntityPtr entityCamera( EntityCreate(AllocatorGetHeap()) );
	entityCamera->setName(name);
	entityCamera->addTransform();
	entityCamera->addComponent( camera );
	entityCamera->addComponent( cameraController );

#if 0
#ifdef VAPOR_AUDIO_OPENAL
	ComponentPtr listener( new Listener() );
	entityCamera->addComponent( listener );
#endif
#endif

	return entityCamera;
}

//-----------------------------------//

void SceneDocument::onRender()
{
	RenderView* view = viewFrame->getView();

	if( !view->getCamera() )
		viewFrame->switchToDefaultCamera();

	const CameraPtr& camera = view->getCamera();
	if( !camera ) return;
	
	camera->setView( view );

	RenderBlock block;
	camera->cull( block, scene );
	camera->cull( block, editorScene );
	camera->render(block);

#if 0
#ifdef VAPOR_PHYSICS_BULLET
	PhysicsManager* physics = engine->getPhysicsManager();
	physics->drawDebugWorld();
#endif
#endif
}

//-----------------------------------//

void SceneDocument::onUpdate( float delta )
{
	GetEngine()->update( delta );
	
	if(editorScene)
		editorScene->update( delta );
	//eventManager->onSceneUpdate();

	if(getRenderWindow()->isCursorVisible() && cameraController->getEnabled())
	{
		LogDebug("Camera failsafe, this should not happen");
		cameraController->setEnabled(false);
	}
}

//-----------------------------------//

} } // end namespaces