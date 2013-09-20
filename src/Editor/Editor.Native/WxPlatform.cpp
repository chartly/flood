/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#include "WxPlatform.h"

NAMESPACE_EDITOR_BEGIN

//-----------------------------------//

WxPlatform::WxPlatform()
    : windowManager(this)
{
}

//-----------------------------------//

void WxPlatform::init()
{
    wxApp::SetInstance(new WxApp());

    if (!wxEntryStart(0, 0))
        return;

    wxTheApp->OnInit();

    wxImage::AddHandler( new wxPNGHandler() );
}

//-----------------------------------//

WxPlatform::~WxPlatform()
{
    wxTheApp->OnExit();
    wxEntryCleanup();
}

//-----------------------------------//

void WxPlatform::update()
{
    wxTheApp->OnRun();
}

//-----------------------------------//

WindowManager* WxPlatform::getWindowManager()
{
    return &windowManager;
}

//-----------------------------------//

InputManager* WxPlatform::getInputManager()
{
    return &inputManager;
}

//-----------------------------------//

NAMESPACE_EDITOR_END

FLD_NAMESPACE::WxPlatform* CreateWxPlatformManager()
{
    using namespace FLD_NAMESPACE;
    return AllocateHeap(WxPlatform);
}