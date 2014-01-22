/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "CS350/API.h"
#include "Engine/PlatformManager.h"
#include "Engine/Window/WindowManager.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Input/Keyboard.h"
#include "Engine/Input/Mouse.h"

namespace dit
{
    class GLWindow;

    class GLWindowManager : public WindowManager
    {
    public:
        GLWindowManager();
        virtual ~GLWindowManager();

        Window* createWindow(const WindowSettings& settings) override;
        Window* openFileDialog(const String& wildcard, FileDialogFlags flags) override              { return nullptr; }
        Window* openDirectoryDialog(const String& wildcard, DirectoryDialogFlags flags) override    { return nullptr; }

        Array<GLWindow*> windows;

        static void OnFrameBufferResize(GLFWwindow * window, int32 width, int32 height);
    };

    class GLInputManager : public InputManager
    {
    public:
        GLInputManager();
        virtual ~GLInputManager();

        virtual Keyboard* getKeyboard() override;
        virtual Mouse* getMouse() override;

        Keyboard* keyboard;
        Mouse* mouse;

        static void OnKeyboardInput(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods);
        static void OnCharInput(GLFWwindow* window, uint32 key);
        static void OnMouseEnterWindow(GLFWwindow * window, int32 enteringWindow);
        static void OnMouseMove(GLFWwindow * window, double x, double y);
        static void OnMouseButton(GLFWwindow * window, int32 button, int32 action, int32 mods);
        static void OnMouseScroll(GLFWwindow * window, double xOffset, double yOffset);

        static Keys ConvertFromGLFWKey(int32 key);
        static MouseButton ConvertFromGLFWMouseButton(int32 button);
    };

    class GLPlatform : public PlatformManager
    {
    public:
        virtual ~GLPlatform();

        virtual void init() override;
        virtual void update() override;

        virtual WindowManager* getWindowManager() override;
        virtual InputManager* getInputManager() override;

        GLWindowManager* windows = nullptr;
        GLInputManager* input = nullptr;

        static void OnError(int32 error, const char * desc);
    };
}