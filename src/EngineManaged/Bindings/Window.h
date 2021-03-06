/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/

#pragma once

#include "CppSharp.h"
#include <Engine/Window/Window.h>
#include "RenderTarget.h"

namespace Flood
{
    enum struct WindowStyles;
    ref class InputManager;
    ref class RenderContext;
    ref class Window;
    value struct RenderContextSettings;
    value struct Vector2i;
    value struct WindowSettings;

    /// <summary>
    /// Represents window styles as bit-flags.
    /// </summary>
    [System::Flags]
    public enum struct WindowStyles
    {
        None = 0,
        /// <summary> A top-level window is a window that is not a child. </summary>
        TopLevel = 2,
        /// <summary> A miniframe is a window with minimal chrome. </summary>
        MiniFrame = 4,
        /// <summary> A borderless window has no chrome. </summary>
        Borderless = 8,
        /// <summary> Fullscreen windows take the full space of the screen. </summary>
        Fullscreen = 16
    };

    /// <summary>
    /// Represents window settings.
    /// </summary>
    public value struct WindowSettings
    {
    public:

        WindowSettings(::WindowSettings* native);
        WindowSettings(System::IntPtr native);
        WindowSettings(unsigned short width, unsigned short height, System::String^ title, Flood::WindowStyles styles);

        unsigned short Width;
        unsigned short Height;
        System::String^ Title;
        System::IntPtr Handle;
        Flood::WindowStyles Styles;
    };

    /// <summary>
    /// Represents onscreen display areas in the target window system.
    /// </summary>
    public ref class Window : Flood::RenderTarget
    {
    public:

        Window(::Window* native);
        Window(System::IntPtr native);
        /// <summary>
        /// Creates a new window with the given settings.
        /// </summary>
        Window(Flood::WindowSettings settings);

        property System::String^ Title
        {
            void set(System::String^);
        }
        property bool CursorVisible
        {
            void set(bool);
        }
        property bool CursorCapture
        {
            void set(bool);
        }
        property Flood::Vector2i CursorPosition
        {
            Flood::Vector2i get();
        }
        property Flood::InputManager^ Input
        {
            Flood::InputManager^ get();
        }
        property Flood::Settings Settings
        {
            Flood::Settings get();
        }
    private:
        delegate void _IdleDelegate();
        _IdleDelegate^ _IdleDelegateInstance;
        void _IdleRaise();
        System::Action^ _Idle;
    public:
        event System::Action^ Idle
        {
            void add(System::Action^ evt);
            void remove(System::Action^ evt);
            void raise();
        }
    private:
        delegate void _RenderDelegate();
        _RenderDelegate^ _RenderDelegateInstance;
        void _RenderRaise();
        System::Action^ _Render;
    public:
        event System::Action^ Render
        {
            void add(System::Action^ evt);
            void remove(System::Action^ evt);
            void raise();
        }
    private:
        delegate void _WindowCloseDelegate();
        _WindowCloseDelegate^ _WindowCloseDelegateInstance;
        void _WindowCloseRaise();
        System::Action^ _WindowClose;
    public:
        event System::Action^ WindowClose
        {
            void add(System::Action^ evt);
            void remove(System::Action^ evt);
            void raise();
        }
    private:
        delegate void _WindowFocusChangeDelegate(bool _0);
        _WindowFocusChangeDelegate^ _WindowFocusChangeDelegateInstance;
        void _WindowFocusChangeRaise(bool _0);
        System::Action<bool>^ _WindowFocusChange;
    public:
        event System::Action<bool>^ WindowFocusChange
        {
            void add(System::Action<bool>^ evt);
            void remove(System::Action<bool>^ evt);
            void raise(bool _0);
        }
        /// <summary>
        /// Creates a new render context.
        /// </summary>
        virtual Flood::RenderContext^ CreateContext(Flood::RenderContextSettings _1);

        /// <summary>
        /// Updates the window content.
        /// </summary>
        virtual void Update() override;

        /// <summary>
        /// Sets this rendering target as the current.
        /// </summary>
        virtual void MakeCurrent() override;

        /// <summary>
        /// Shows/hides the window.
        /// </summary>
        virtual void Show(bool visible);

        /// <summary>
        /// Handles the event loop of the window.
        /// </summary>
        virtual bool PumpEvents();

        /// <summary>
        /// Gets the visibility of the mouse cursor.
        /// </summary>
        virtual bool IsCursorVisible();

        /// <summary>
        /// Sets the cursor position on screen.
        /// </summary>
        virtual void SetCursorPosition(int x, int y);

        /// <summary>
        /// Gets if the window has focus.
        /// </summary>
        virtual bool HasFocus();

        virtual bool Equals(System::Object^ object) override;

        virtual int GetHashCode() override;

    };
}
