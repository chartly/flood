/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "Graphics/API.h"
#include "Engine/Input/InputManager.h"
#include "Engine/Input/Keyboard.h"
#include "Engine/Input/Mouse.h"

namespace dit
{
    /**
    * Represents window styles as bit-flags.
    */
    enum class WindowStyles
    {
        None = 0,
        /// A top-level window is a window that is not a child.
        TopLevel = 1 << 1,
        /// A miniframe is a window with minimal chrome.
        MiniFrame = 1 << 2,
        /// A borderless window has no chrome.
        Borderless = 1 << 3,
        /// Fullscreen windows take the full space of the screen.
        Fullscreen = 1 << 4,
    };

    /**
    * Represents window settings.
    */
    class WindowSettings
    {
    public:
        WindowSettings() = default;
        WindowSettings(uint16 w, uint16 h, const char * t, void* hdl, WindowStyles s)
            : width(w), height(h), title(t), handle(hdl), styles(s)
        {}

        uint16 width;
        uint16 height;
        String title;
        void* handle;
        WindowStyles styles;
    };

    class GLWindow;
    class GLWindowManager
    {
    public:
        GLWindowManager();
        virtual ~GLWindowManager();

        GLWindow* create(const WindowSettings& settings);

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

    class GLPlatform
    {
    public:
        ~GLPlatform();

        void init();
        void update();

        GLWindowManager* windows = nullptr;
        GLInputManager* input = nullptr;

        static void OnError(int32 error, const char * desc);
    };
}