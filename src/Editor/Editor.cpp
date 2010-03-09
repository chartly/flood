/************************************************************************
*
* vapor3D Editor � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#include "PCH.h"
#include "Editor.h"
#include "EditorIcons.h"
#include "Gizmo.h"

// Editor modes
#include "GizmoMode.h"
#include "TerrainMode.h"

namespace vapor { namespace editor {

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Editor_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Editor_About = wxID_ABOUT,

	// Toolbar buttons
	Toolbar_Save = 12879,
	Toolbar_ToggleScene,
	Toolbar_ToogleConsole,
	Toolbar_ToogleGrid,
	Toolbar_TooglePlay,
	Toolbar_ToogleSidebar,
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(EditorFrame, wxFrame)
    EVT_MENU(Editor_Quit,  EditorFrame::OnQuit)
    EVT_MENU(Editor_About, EditorFrame::OnAbout)
	EVT_MENU(wxID_ANY, EditorFrame::OnToolbarButtonClick)
	EVT_KEY_DOWN(EditorFrame::OnKeyDown)
END_EVENT_TABLE()

//-----------------------------------//

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type.
IMPLEMENT_APP(EditorApp)

//-----------------------------------//

// 'Main program' equivalent: the program execution "starts" here
bool EditorApp::OnInit()
{
    // Call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future.
    if ( !wxApp::OnInit() )
	{
        return false;
	}

	// Register a PNG image handler (so toolbars icons can be decoded).
	wxImage::AddHandler(new wxPNGHandler);

    // Create the main application window, provide a default size and show it.
	// Unlike simple controls, frames are not shown automatically when created.
    EditorFrame* frame = new EditorFrame("vaporEditor - level editor for vapor");
	frame->SetSize(750, 500);
    frame->Show(true);

	// Set our created window as the top-level window. This provides a few
	// advantages. The wxWidgets manual says the first created frame will
	// be the default top-level window, but better be safe than sorry. :)
	SetTopWindow( frame );

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

//-----------------------------------//

// frame constructor
EditorFrame::EditorFrame(const wxString& title)
       : wxFrame(nullptr, wxID_ANY, title), engine( nullptr ), 
	   currentMode( nullptr )
{
    // set the frame icon
    SetIcon( wxIcon( "editor" ) );
	
	// initialize the engine
	initEngine();
	
	sizer = new wxBoxSizer( wxHORIZONTAL );
	sizer->Add( viewport, 1, wxEXPAND|wxALL );

	// create window basic widgets
	createModes();
	createMenus();
	createToolbar();
	createStatusbar();
	createNotebook();
	createScene();

	toolBar->Realize();
	SetSizerAndFit( sizer );
	vaporCtrl->SetFocus();
}

//-----------------------------------//

EditorFrame::~EditorFrame()
{
	foreach( const Mode* mode, editorModes )
		delete mode;

	vaporCtrl->Destroy();
	delete engine;
}

//-----------------------------------//

void EditorFrame::initEngine()
{
	engine = new vapor::Engine("vaporEditor", nullptr, false);
	engine->init( false );

	viewport = new Viewport( engine, this );
	vaporCtrl = viewport->vaporCtrl;

	engine->getRenderDevice()->init();
	engine->getVFS()->mountDefaultLocations();

	input::Mouse* mouse = engine->getInputManager()->getMouse();
	mouse->onMouseButtonPress += fd::bind( &EditorFrame::onMouseClick, this );
}

//-----------------------------------//

void EditorFrame::createModes()
{
	editorModes.push_back( new GizmoMode(this) );
	editorModes.push_back( new TerrainMode(this) );
}

//-----------------------------------//

void EditorFrame::createScene()
{
	ScenePtr scene = engine->getSceneManager();
	ResourceManager* rm = engine->getResourceManager();

	ProgramPtr diffuse( new GLSL_Program( 
			rm->loadResource< GLSL_Shader >( "diffuse.vs" ),
			rm->loadResource< GLSL_Shader >( "diffuse.fs" ) ) );

	ProgramPtr tex( new GLSL_Program( 
			rm->loadResource< GLSL_Shader >( "tex.vs" ),
			rm->loadResource< GLSL_Shader >( "tex.fs" ) ) );

	ProgramManager::getInstance().registerProgram( "diffuse", diffuse );
	ProgramManager::getInstance().registerProgram( "tex", tex );

	ProgramPtr toon( new GLSL_Program( 
			rm->loadResource< GLSL_Shader >( "toon.vs" ),
			rm->loadResource< GLSL_Shader >( "toon.fs" ) ) );

	ProgramManager::getInstance().registerProgram( "toon", toon );

	ProgramPtr tex_toon( new GLSL_Program( 
			rm->loadResource< GLSL_Shader >( "tex_toon.vs" ),
			rm->loadResource< GLSL_Shader >( "tex_toon.fs" ) ) );
	
	ProgramManager::getInstance().registerProgram( "tex_toon", tex_toon );

	MaterialPtr mat( new Material( "GridMaterial", diffuse ) );
	NodePtr grid( new Node( "EditorGrid" ) );
	grid->addComponent( TransformPtr( new Transform() ) );
	grid->addComponent( ComponentPtr( new Grid( mat ) ) );
	scene->add( grid );
	
	MaterialPtr matSun( new Material( "SunBlend" ) );
	matSun->setProgram( "tex" );
	matSun->setTexture( 0, "moon.png" );
	matSun->setBlending( BlendingOperationSource::SourceAlpha,
		BlendingOperationDestination::OneMinusSourceAlpha );
	matSun->setBackfaceCulling( false );
	
	RenderablePtr sunQuad( new render::Quad( 100.0f, 100.0f ) );
	sunQuad->setMaterial( matSun );

	GeometryPtr geom( new Geometry() );
	geom->addRenderable( sunQuad, RenderGroup::Transparency );

	NodePtr sun( new Node( "Sun" ) );
	sun->addComponent( TransformPtr( new Transform() ) );
	sun->addComponent( geom );
	sun->addComponent( BillboardPtr( new Billboard( viewport->camera ) ) );
	scene->add( sun );

	SkydomePtr skydome( new Skydome( mat ) );
	skydome->setSunNode( sun );

	NodePtr sky( new Node( "Sky" ) );
	sky->addComponent( TransformPtr( new Transform() ) );	
	sky->addComponent( skydome );
	scene->add( sky );

	NodePtr lnode( new Node( "Light" ) );
	lnode->addComponent( TransformPtr( new Transform() ) );
	LightPtr light( new Light( LightType::Point ) );
	light->diffuseColor = Colors::Red;
	light->ambientColor = Colors::Yellow;
	lnode->addComponent( light );
	scene->add( lnode );

	MaterialPtr cellMaterial( new Material("CellMaterial") );
	cellMaterial->setTexture( 0, "PineTrunk.png" );
	cellMaterial->setProgram( "tex_toon" );

	TerrainSettings settings;
	settings.CellSize = 512;
	settings.NumberTiles = 32;
	settings.MaxHeight = 100;
	settings.Material = cellMaterial;

	TerrainPtr terrain( new Terrain( settings ) );

	NodePtr terreno( new Node( "Terreno" ) );
	terreno->addComponent( TransformPtr( new Transform() ) );
	terreno->addComponent( terrain );
	scene->add( terreno );

	ImagePtr heightmap = rm->loadResource< Image >( "height2.png" );
	const CellPtr& cell = terrain->createCell( heightmap, 0, 0 );
}

//-----------------------------------//

void EditorFrame::createNotebook()
{
	wxImageList* img = new wxImageList(16, 16, false, 5);
	img->Add(wxMEMORY_BITMAP(sitemap_color));
	img->Add(wxMEMORY_BITMAP(package));
	img->Add(wxMEMORY_BITMAP(world));

	notebookCtrl = new wxNotebook( this, wxID_ANY );
	notebookCtrl->AssignImageList( img );

	//-----------------------------------//

	wxPanel* panelScene = new wxPanel( notebookCtrl, wxID_ANY );
	
	wxBoxSizer* panelSceneSizer = new wxBoxSizer( wxVERTICAL );
	
	sceneTreeCtrl = new SceneTreeCtrl(engine, panelScene, ID_SceneTree,
		wxDefaultPosition, wxSize(230, -1) );

	sceneTreeCtrl->onItemSelected += fd::bind( &EditorFrame::OnNodeSelected, this );

	panelSceneSizer->Add( sceneTreeCtrl, 1, wxALL|wxEXPAND, 0 );

	panelScene->SetSizerAndFit( panelSceneSizer );

	int scenePage = notebookCtrl->AddPage( panelScene, wxT("Scene"), true, 0 );

	sizer->Add(notebookCtrl, 0, wxALL|wxEXPAND, 0 );

	//-----------------------------------//

	wxPanel* panelResources = new wxPanel( notebookCtrl, wxID_ANY );
	
	wxBoxSizer* panelResourcesSizer = new wxBoxSizer( wxVERTICAL );
	
	resourceTreeCtrl = new ResourceTreeCtrl(engine, panelResources, ID_SceneTree,
		wxDefaultPosition, wxSize(220, -1) );

	panelResourcesSizer->Add( resourceTreeCtrl, 1, wxALL|wxEXPAND, 0 );

	panelResources->SetSizerAndFit( panelResourcesSizer );

	notebookCtrl->AddPage( panelResources, wxT("Resources"), true, 1 );

	notebookCtrl->SetSelection( scenePage );

	//-----------------------------------//

	terrainPage = new TerrainPage( notebookCtrl );
	notebookCtrl->AddPage( terrainPage, "Terrains", true, 2 );

	notebookCtrl->ChangeSelection( scenePage );
}

//-----------------------------------//

void EditorFrame::createStatusbar()
{
#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    //CreateStatusBar(2);
	//SetStatusText("vaporEngine (FPS: OVER 9000!)");
#endif // wxUSE_STATUSBAR
}

//-----------------------------------//

void EditorFrame::createMenus()
{
#if wxUSE_MENUS
    // create a menu bar
    wxMenu* fileMenu = new wxMenu;
	fileMenu->Append(Editor_Quit, "E&xit\tAlt-X", "Quit this program");

    // the "About" item should be in the help menu
    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(Editor_About, "&About...\tF1", "Show about dialog");

    // now append the freshly created menu to the menu bar...
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS
}

//-----------------------------------//

void EditorFrame::createToolbar()
{
	toolBar = CreateToolBar( wxTB_HORIZONTAL, wxID_ANY );

	// --------------
	// Project tools
	// --------------

	toolBar->AddTool( wxID_ANY, "New", wxMEMORY_BITMAP(page_empty) );
	toolBar->AddTool( wxID_ANY, "Open", wxMEMORY_BITMAP(folder_explore) ); 
	toolBar->AddTool( Toolbar_Save, "Save", wxMEMORY_BITMAP(disk) );
	
	toolBar->AddSeparator();

	toolBar->AddTool( wxID_ANY, "Undo", wxMEMORY_BITMAP(arrow_undo) );
	toolBar->AddTool( wxID_ANY, "Redo", wxMEMORY_BITMAP(arrow_redo) );

	toolBar->AddSeparator();

	toolBar->AddTool( Toolbar_ToogleConsole, "Console", 
		wxMEMORY_BITMAP(application_xp_terminal), "Open/close the scripting console" ); 

	toolBar->AddTool( Toolbar_ToogleGrid, "Grid", 
		wxMEMORY_BITMAP(grid_icon), "Show/hide the editor grid", wxITEM_CHECK );

	toolBar->ToggleTool( Toolbar_ToogleGrid, true );

	toolBar->AddTool( Toolbar_TooglePlay, "Play", wxMEMORY_BITMAP(resultset_next), 
		"Enable/disable Play mode", wxITEM_CHECK );

	// --------------------
	// Mode-specific tools
	// --------------------

	foreach( Mode* mode, editorModes )
	{
		mode->onModeInit( toolBar, modeIds );
	}

	// --------------
	// UI tools
	// --------------

	toolBar->AddSeparator();

	toolBar->AddTool( Toolbar_ToogleSidebar, "Show/hide sidebar", 
		wxMEMORY_BITMAP(application_side_tree_right), "Shows/hides the sidebar" ); 

	codeEvaluator = new ConsoleFrame( engine, this, "Scripting Console" );
}

//-----------------------------------//
// event handlers
//-----------------------------------//

void EditorFrame::OnNodeSelected(wxTreeItemId old, wxTreeItemId id)
{
	if( old )
	{
		// Remove the Gizmo component.
		const NodePtr& node = sceneTreeCtrl->getEntity( old );
		
		if( node )
		{
			const TransformPtr& tr = node->getTransform();
			tr->setDebugRenderableVisible( false );
			node->removeComponent( "Gizmo" );
		}
	}

	const NodePtr& node = sceneTreeCtrl->getEntity( id );
	if( !node ) return;

	const TransformPtr& tr = node->getTransform();
	tr->setDebugRenderableVisible( true );
	GeometryPtr gizmo( new Gizmo( tr->getBoundingVolume().getCenter() ) ); 	
	node->addComponent(gizmo);
}

//-----------------------------------//

void EditorFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true forces the frame to close.
    Close(true);
}

//-----------------------------------//

void EditorFrame::OnKeyDown(wxKeyEvent& event)
{
	const ScenePtr& scene = engine->getSceneManager();
	
	if( event.GetKeyCode() == 'g' )
	{
		const NodePtr& grid = scene->getEntity( "EditorGrid" );
		if( !grid ) return;
		grid->setVisible( !grid->isVisible() );
	}
}

//-----------------------------------//

void EditorFrame::OnToolbarButtonClick(wxCommandEvent& event)
{
	int id = event.GetId();

	// If the id is registered to a mode, then switch mode...
	if( modeIds.find(id) != modeIds.end() )
	{
		onModeSwitch( modeIds[id], id );
		return;
	}

	switch(id) 
	{ 
	case Toolbar_Save:
	{
		// Ask for file name to save as.
		wxFileDialog fc( this, wxFileSelectorPromptStr, wxEmptyString,
			wxEmptyString, "Scene files (*.scene)|*.scene", wxFC_SAVE );
		
		if( fc.ShowModal() != wxID_OK )
			return;

		// Serialize scene to JSON.
		Json::Value scene;
		engine->getSceneManager()->serialize( scene );

		// Save it to a file.
		std::string fn( fc.GetFilename() );
		serializeToFile( scene, fn );
		break;
	}

	case Toolbar_ToogleConsole:
	{
		codeEvaluator->Show( !codeEvaluator->IsShown() );
		codeEvaluator->SetFocus();
		break;
	}
	
	case Toolbar_ToogleGrid:
	{
		NodePtr grid = engine->getSceneManager()->getEntity( "EditorGrid" );
		if( grid ) grid->setVisible( !grid->isVisible() );
		break;
	}

	case Toolbar_TooglePlay:
	{
		//physics::PhysicsManager* pm = engine->getPhysicsManager();
		//if( pm ) pm->setSimulationEnabled( !pm->getSimulationEnabled() );
		break;
	}

	case Toolbar_ToogleSidebar:
	{
		wxSize szNew = viewport->GetClientSize();
		const wxSize& szNB = notebookCtrl->GetClientSize();

		if( notebookCtrl->IsShown() )
		{
			notebookCtrl->Hide();
		}
		else
		{
			notebookCtrl->Show();
			szNew.SetWidth( szNew.GetWidth() + szNB.GetWidth() );
		}

		SetClientSize( szNew );
		Layout();
	}
	}
}

//-----------------------------------//

void EditorFrame::onModeSwitch( Mode* newMode, int id )
{
	if( !newMode ) return;

	if( currentMode )
		currentMode->onModeExit();
	
	currentMode = newMode;
	currentMode->onModeEnter( id );
}

//-----------------------------------//

void EditorFrame::onMouseClick( const MouseButtonEvent& mbe )
{
	if( !currentMode ) return;
	currentMode->onMouseClick( mbe.x, mbe.y );
}

//-----------------------------------//

void EditorFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxFrame* about = new wxFrame(this, wxID_ANY, "About vapor3D Editor", wxDefaultPosition, wxDefaultSize, 
		wxCAPTION|wxCLOSE_BOX|wxFRAME_FLOAT_ON_PARENT|wxFRAME_TOOL_WINDOW|wxSYSTEM_MENU|wxTAB_TRAVERSAL );

	wxBoxSizer* bSizer1 = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer* bSizer2 = new wxBoxSizer( wxVERTICAL );

	wxPanel* m_panel1 = new wxPanel( about );

	wxStaticBitmap* m_bitmap1 = new wxStaticBitmap( m_panel1, wxID_ANY, wxMEMORY_BITMAP(::about) );
	bSizer2->Add( m_bitmap1, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 0 );

	wxString aboutText( "This software is � 2009-2010 Jo�o Matos and the rest of the vapor3D Team.\n\n"
		"vapor3D Editor uses some free software packages: wxWidgets (wxWidgets.org), Lua (lua.org),\n"
		"Bullet (bulletphysics.com), Boost (boost.org), zlib (zlib.org) and the list goes on.\n\n"
		"Check the documentation provided with the software for more details." );

	wxStaticText* m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, aboutText );
	m_staticText2->Wrap( -1 );
	bSizer2->Add( m_staticText2, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	wxBoxSizer* bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticLine* m_staticline1 = new wxStaticLine( m_panel1, wxID_ANY );
	bSizer3->Add( m_staticline1, 1, wxALL|wxALIGN_CENTER_VERTICAL, 10 );
	
	wxHyperlinkCtrl* m_hyperlink1 = new wxHyperlinkCtrl( m_panel1, wxID_ANY,
		"vapor3D (http://www.vapor3d.org)", "http://www.vapor3d.org" );
	bSizer3->Add( m_hyperlink1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	//wxStaticText* m_hyperlink1 = new wxStaticText( m_panel1, wxID_ANY,
	//	"http://www.vapor3d.org" );
	//bSizer3->Add( m_hyperlink1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxStaticLine* m_staticline2 = new wxStaticLine( m_panel1, wxID_ANY );
	bSizer3->Add( m_staticline2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 10 );
	
	bSizer2->Add( bSizer3, 1, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();

	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND, 5 );
	
	about->SetSizer( bSizer1 );
	about->Layout();
	bSizer1->Fit( about );

	about->Show(true);
}

//-----------------------------------//

} } // end namespaces