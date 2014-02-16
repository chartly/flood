/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "Graphics/API.h"

//-----------------------------------//

namespace dit {

    /**
     *	OpenGL window implementation using GLFW to wrap platform-level
     *	windowing and GLEW to wrap the graphics context.
     */
    class GLWindow
    {
    public:
        GLWindow(GLFWwindow * window, GLEWContext* context);
        virtual ~GLWindow();

        void update();
        void makeCurrent();
        void show(bool visible = true);

        void setTitle(const String& title);
        void setCursorVisible(bool state);
        bool isCursorVisible() const;

        GLFWwindow* window;
        GLEWContext* context;
    };
}