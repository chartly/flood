/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "Graphics/API.h"
#include "Graphics/GL/GLWindow.h"
#include "Core/Log.h"

namespace dit {

    //-----------------------------------//

    GLWindow::GLWindow(GLFWwindow* win, GLEWContext* con)
        : window(win)
        , context(con)
    {}

    GLWindow::~GLWindow()
    {
        glfwDestroyWindow(window);
    }

    void GLWindow::update() 
    {}

    void GLWindow::makeCurrent()
    {
        glfwMakeContextCurrent(window);
    }

    void GLWindow::show(bool visible) 
    {
        if (visible)
            glfwShowWindow(window);
        else
            glfwHideWindow(window);
    }

    void GLWindow::setTitle(const String& title) 
    {
        glfwSetWindowTitle(window, title.c_str());
        LogInfo( "Changing window title to '%s'", title.c_str() );
    }

    void GLWindow::setCursorVisible(bool state)
    {
        if(state)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    bool GLWindow::isCursorVisible() const
    {
        return glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
    }

    //-----------------------------------//

}