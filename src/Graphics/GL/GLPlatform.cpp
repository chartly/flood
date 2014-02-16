/************************************************************************
*
* Corbin Hart � (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "Graphics/API.h"
#include "Graphics/GL/GLApp.h"
#include "Graphics/GL/GLPlatform.h"
#include "Graphics/GL/GLWindow.h"
#include "Graphics/GL/GLApp.h"

#include "Engine/Engine.h"

#include <sstream>

namespace dit {

    //-----------------------------------//

    GLWindowManager::GLWindowManager()
    {}

    GLWindowManager::~GLWindowManager()
    {
        for(auto w : windows)
        {
            Deallocate(w);
        }
    }
    
    GLWindow* GLWindowManager::create(const WindowSettings& settings)
    {
        LogInfo("Creating a glfw window..");

        // set the appropriate window hints / attributes
#pragma TODO("Respect ALL window hints from the provided settings.")
        GLFWmonitor* monitor = nullptr;
        switch(settings.styles)
        {
        case WindowStyles::Fullscreen:
            monitor = glfwGetPrimaryMonitor();
            break;
        case WindowStyles::Borderless:
            glfwWindowHint(GLFW_DECORATED, GL_FALSE);
            break;
        case WindowStyles::MiniFrame:
            glfwWindowHint(GLFW_DECORATED, GL_TRUE);
            break;
        case WindowStyles::TopLevel:
            break;
        case WindowStyles::None:
            break;
        default:
            assert("Invalid value specified for WindowSettings::styles." && false);
        }

        // create the glfw window
        auto window = glfwCreateWindow(settings.width, settings.height, settings.title.c_str(), monitor, nullptr);
        if(!window)
        {
            LogError("Failed to create a glfw window.");
            return nullptr;
        }

        // hook glfw input
        glfwSetCharCallback(window, GLInputManager::OnCharInput);
        glfwSetKeyCallback(window, GLInputManager::OnKeyboardInput);
        glfwSetCursorEnterCallback(window, GLInputManager::OnMouseEnterWindow);
        glfwSetCursorPosCallback(window, GLInputManager::OnMouseMove);
        glfwSetMouseButtonCallback(window, GLInputManager::OnMouseButton);
        glfwSetScrollCallback(window, GLInputManager::OnMouseScroll);
        glfwSetFramebufferSizeCallback(window, GLWindowManager::OnFrameBufferResize);

        glfwMakeContextCurrent(window);

        if(fldApp()->context == nullptr)
        {
            fldApp()->context = Allocate(AllocatorGetHeap(), GLEWContext);

            glewExperimental = true;
            auto res = glewInit();
            if(res == GLEW_OK)
                 LogInfo("glew initialized...");
            else
            {
                String msg, err;
                err = (char*)glewGetErrorString(res);
                mini::format(msg, "glew failed to initialize: %0", err.c_str());
                LogError(msg.c_str());
            }
        }

        // alloc the adapter class
        auto glWindow = Allocate(AllocatorGetHeap(), GLWindow, window, fldApp()->context);
        return glWindow;
    }

    void GLWindowManager::OnFrameBufferResize(GLFWwindow * window, int32 width, int32 height)
    {
        LogInfo("OnFrameBufferResize not yet implemented.");
    }

    //-----------------------------------//

    GLInputManager::GLInputManager()
    {
        keyboard = Allocate(AllocatorGetHeap(), Keyboard);
        mouse = Allocate(AllocatorGetHeap(), Mouse);
    }

    GLInputManager::~GLInputManager()
    {
        Deallocate(mouse);
        Deallocate(keyboard);
    }

    Keyboard* GLInputManager::getKeyboard()
    {
        return keyboard;
    }

    Mouse* GLInputManager::getMouse()
    {
        return mouse;
    }

    void GLInputManager::OnCharInput(GLFWwindow* window, uint32 key)
    {
#ifdef ENABLE_ANTTWEAKBAR
        // forward to anttweakbar
        TwEventCharGLFW((int32)key, GLFW_PRESS);
#endif
    }

    void GLInputManager::OnKeyboardInput(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods)
    {
#ifdef ENABLE_ANTTWEAKBAR
        // forward to anttweakbar
        TwEventCharGLFW(key, scancode);
#endif

        // forward to the engine
        KeyEvent ke = (scancode != GLFW_RELEASE) ? (KeyboardEventType::KeyPressed) : KeyboardEventType::KeyReleased;
        ke.keyCode = ConvertFromGLFWKey(key);
        ke.altPressed = mods & GLFW_MOD_ALT;
        ke.shiftPressed = mods & GLFW_MOD_SHIFT;
        ke.ctrlPressed = mods & GLFW_MOD_CONTROL;

        auto keyboard = fldApp()->platform.input->keyboard;
        keyboard->processEvent(ke);
    }

    void GLInputManager::OnMouseEnterWindow(GLFWwindow * window, int32 enteringWindow)
    {}

    void GLInputManager::OnMouseMove(GLFWwindow * window, double x, double y)
    {
#ifdef ENABLE_ANTTWEAKBAR
        // forward to anttweakbar
        TwEventMousePosGLFW((int32)x, (int32)y);
#endif

        // forward to the engine
        MouseMoveEvent me;
        me.x = (int16)x;
        me.y = (int16)y;

        auto mouse = fldApp()->platform.input->keyboard;
        mouse->processEvent(me);
    }

    void GLInputManager::OnMouseButton(GLFWwindow * window, int32 button, int32 action, int32 mods)
    {
#ifdef ENABLE_ANTTWEAKBAR
        // forward to anttweakbar
        TwEventMouseButtonGLFW(button, action);
#endif

        // forward to the engine
        auto mouse = fldApp()->platform.input->mouse;

        MouseButtonEvent mb( (action == GLFW_PRESS) ? MouseEventType::MousePress : MouseEventType::MouseRelease );
        mb.x = mouse->mouseInfo.x;
        mb.y = mouse->mouseInfo.y;
        mb.button = ConvertFromGLFWMouseButton(button);

        mouse->processEvent(mb);
    }

    void GLInputManager::OnMouseScroll(GLFWwindow * window, double xOffset, double yOffset)
    {
#ifdef ENABLE_ANTTWEAKBAR
        // forward to anttweakbar
        TwEventMouseWheelGLFW((int)xOffset);
#endif
    }

    //-----------------------------------//

    GLPlatform::~GLPlatform()
    {
        Deallocate(input);
        Deallocate(windows);

        glfwTerminate();
    }

    void GLPlatform::init()
    {
        // hook up framework adapters
        glfwSetErrorCallback(GLPlatform::OnError);

        // initialize glfw
        if(!glfwInit())
        {
            LogError("failed to initialize glfw.");
            return;
        }

        LogInfo("initialized glfw.");

        // alloc system adapters
        windows = Allocate(AllocatorGetHeap(), GLWindowManager);
        input = Allocate(AllocatorGetHeap(), GLInputManager);
    }

    void GLPlatform::update()
    {
        // update each window
        for(auto w : windows->windows)
        {
            w->update();
            //if(w->shouldClose)
            //  windowsToRemove.pushBack(w)
        }

        input->keyboard->reset();
        glfwPollEvents();
    }

    void GLPlatform::OnError(int32 error, const char * desc)
    {
        String msg;
        mini::format(msg, "glfw error: %0 : \"%1\"", error, desc);
        LogError(msg.c_str());
    }

    //-----------------------------------//
}