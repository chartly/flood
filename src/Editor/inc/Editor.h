/************************************************************************
*
* vapor3D Editor � (2008-2010)
*
*	<http://www.vapor3d.org>
*
************************************************************************/

#pragma once

class wxFourWaySplitter;

namespace vapor { namespace editor {

//-----------------------------------//

class Plugin;
class PluginManager;
class PluginManagerFrame;
class EditorInputManager;
class UndoManager;
class Viewframe;
class Events;

//-----------------------------------//

// Define a new application type, each program should derive a class from wxApp
class EditorApp : public wxApp
{
public:

    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
	//virtual void OnUnhandledException();
	//virtual bool OnExceptionInMainLoop();
};

//-----------------------------------//

// Define a new frame type: this is going to be our main frame
class EditorFrame : public wxFrame, public wxDropTarget
{
public:

    EditorFrame(const wxString& title);
	virtual ~EditorFrame();
	
	// Refreshes the main view.
	void redrawView();

	// Gets the engine instance.	
	GETTER(Engine, Engine* const, engine)

	// Gets the main viewframe instance.
	GETTER(MainViewframe, Viewframe* const, viewframe)

	// Gets the editor scene instance.
	GETTER(EditorScene, ScenePtr, editorScene)

	// Gets the Undo/Redo manager instance.
	GETTER(UndoManager, UndoManager*, undoManager)

	// Gets the Undo/Redo manager instance.
	GETTER(PluginManager, PluginManager*, pluginManager)

	// Gets the events manager instance.
	GETTER(EventManager, Events*, eventManager)

	// Gets the notebook control.
	GETTER(Notebook, wxNotebook*, notebookCtrl)

	// Gets the toolbar control.
	GETTER(Toolbar, wxToolBar*, toolBar)

	// Gets the drag and drop coords.
	GETTER(DropCoords, Vector2, dropCoords)

	// Gets the player camera.
	CameraPtr getPlayerCamera() const;

	// Switches the editor to play mode.
	void switchPlayMode(bool switchToPlay);

protected:

	// Creates the main UI layout.
	void createUI();
	void createSplitter();
	void createMenus();
	void createToolbar();
	void createNotebook();

	// Creates the engine instance.
	void createEngine();

	// Creates the views.
	void createViews();

	// Creates the editor managers.
	void createServices();

	// Creates the editor plugins.
	void createPlugins();

	// Creates the editor scene.
	void createScene();

	// Creates the default resources.
	void createResources();

	// Creates a new camera.
	NodePtr createCamera();

	// View callbacks.
	void onUpdate( double delta );
	void onRender();

    // wxWidgets main events.
	void OnIdle(wxIdleEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void OnToolbarButtonClick(wxCommandEvent& event);
	wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);

	// wxWidgets input events.
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void OnMouseEvent(wxMouseEvent& event);

	// Drag and drop coords.
	Vector2 dropCoords;

	// Main engine instance.
	Engine* engine;

	// Manages the editor scene entities.	
	ScenePtr editorScene;

	// Input Management.
	EditorInputManager* inputManager;

	// Plugin Manager.
	PluginManager* pluginManager;
	
	// Plugin Manager Frame.
	PluginManagerFrame* pluginManagerFrame;

	// Undo/Redo operations.
	UndoManager* undoManager;

	// Global Events.
	Events* eventManager;

	// UI widgets.
	wxBoxSizer* sizer;
	wxSplitterWindow* mainSplitter;
	wxFourWaySplitter* viewSplitter;
	wxToolBar* toolBar;
	Viewframe* viewframe;
	wxNotebook* notebookCtrl;
	wxTextDataObject data;
};

//-----------------------------------//

} } // end namespaces