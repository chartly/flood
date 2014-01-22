/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "CS350/API.h"
#include "CS350/GL/GLWindow.h"

namespace dit {

    //-----------------------------------//

    GLWindow::GLWindow(const WindowSettings& settings, GLFWwindow* win, GLEWContext* con)
        : Window(settings)
        , window(win)
        , context(con)
    {}

    GLWindow::~GLWindow()
    {
        glfwDestroyWindow(window);
    }

    RenderContext* GLWindow::createContext(const RenderContextSettings& settings)
    {
        return nullptr;
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

    void GLWindow::setCursorPosition(int32 x, int32 y)
    {
        glfwSetCursorPos(window, (double)x, (double)y);
    }

    Vector2i GLWindow::getCursorPosition() const
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return Vector2i((int32)x, (int32)y);
    }

    //-----------------------------------//

}