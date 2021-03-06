// TODO: This code is now deprecated in favor of GLFW, may come back to it at some point
#if 0

#include "platform/Window.h"
#include "jade/core/Core.h"

namespace Jade {
    std::vector<Window*> Window::s_Windows = std::vector<Window*>();
    std::chrono::steady_clock::time_point Window::s_TimeStarted = std::chrono::high_resolution_clock::now();
    int Window::s_SwapInterval = 0;

    bool Window::s_InitFocused = false;
    bool Window::s_InitIconified = false;
    bool Window::s_InitResizable = true;
    bool Window::s_InitVisible = true;
    bool Window::s_InitDecorated = true;
    bool Window::s_InitAutoIconify = false;
    bool Window::s_InitFloating = false;
    bool Window::s_InitMaximized = false;
    bool Window::s_InitCenterCursor = false;
    bool Window::s_InitTransparentFramebuffer = false;
    bool Window::s_InitHovered = false;
    bool Window::s_InitFocusOnShow = false;
    bool Window::s_InitAllocConsole = false;

    void Window::WindowHint(int hint, int value) {
        switch(hint) {
            case JADE_WH_FOCUSED:
                s_InitFocused = value;
                break;
            case JADE_WH_ICONIFIED:
                s_InitIconified = value;
                break;
            case JADE_WH_RESIZABLE:
                s_InitResizable = value;
                break;
            case JADE_WH_VISIBLE:
                s_InitVisible = value;
                break;
            case JADE_WH_DECORATED:
                s_InitDecorated = value;
                break;
            case JADE_WH_AUTO_ICONIFY:
                s_InitAutoIconify = value;
                break;
            case JADE_WH_FLOATING:
                s_InitFloating = value;
                break;
            case JADE_WH_MAXIMIZED:
                s_InitMaximized = value;
                break;
            case JADE_WH_CENTER_CURSOR:
                s_InitCenterCursor = value;
                break;
            case JADE_WH_TRANSPARENT_FRAMEBUFFER:
                s_InitTransparentFramebuffer = value;
                break;
            case JADE_WH_HOVERED:
                s_InitHovered = value;
                break;
            case JADE_WH_FOCUS_ON_SHOW:
                s_InitFocusOnShow = value;
                break;
            case JADE_WH_ALLOC_CONSOLE:
                s_InitAllocConsole = value;
                break;
            default:
                Log::Warning("Unknown window flag 0x%08x", hint);
                break;
        }
    }

    Window* Window::CreateWindow(int width, int height, const char* title) {
        Window* win = nullptr;
#ifdef _WIN32
        win = Win32Window::CreateWindow(width, height, title);
#endif

        Log::Assert(win != nullptr, "Could not create window!");
        s_Windows.push_back(win);
        return win;
    }

    void* Window::GetWindowHandle(Window* window) {
    #ifdef _WIN32
        Win32Window* win32Window = (Win32Window*)window;
        return win32Window->_GetWindowHandle();
    #else
        return nullptr;
    #endif
    }

    void Window::Show(Window* window) {
        window->_Show();
    }

    int Window::Init() {
        s_TimeStarted = std::chrono::high_resolution_clock::now();
#ifdef _WIN32
        Win32Window::InitWin32();
#endif

        return 0;
    }

    void Window::SwapInterval(int interval) {
        s_SwapInterval = interval;
        for (Window* win : s_Windows) {
            win->UpdateSwapInterval();
        }
    }

    void Window::Destroy(Window* window) {
        window->_Destroy();
    }

    void Window::SwapBuffers(Window* window) {
        window->_SwapBuffers();
    }

    void Window::MakeContextCurrent(Window* win) {
        win->_MakeContextCurrent();
    }

    void Window::SetWindowTitle(Window* win, const char* title) {
        win->_SetWindowTitle(title);
    }

    void Window::PollEvents() {
#ifdef _WIN32
        Win32Window::_PollEvents();
#endif
    }

    float Window::GetTime() {
        auto time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<float>(time - s_TimeStarted).count();
    }

    bool Window::WindowShouldClose(Window* window) {
        return window->m_WindowShouldClose;
    }

    void Window::SetWindowPos(Window* window, int x, int y) {
        if (window == nullptr) {
            Log::Warning("Tried to set window position of nullptr.");
           return;
        }

        window->_SetWindowPos(x, y);
    }

    void Window::SetWindowSize(Window* window, int width, int height) {
        if (window == nullptr) {
            Log::Warning("Tried to set window size of nullptr.");
            return;
        }

        window->_SetWindowSize(width, height);
    }

    void Window::SetWindowIcon(Window* window, int count, const WindowImage* images) {
        if (window == nullptr) {
            Log::Warning("Tried to set window icon of nullptr.");
            return;
        }

        window->_SetWindowIcon(count, images);
    }

    void Window::SetWindowSizeCallback(Window* window, ResizeCallbackFnPt resizeCallback) {
        if (window == nullptr) {
            Log::Warning("Tried to set window size callback of nullptr.");
            return;
        }

        window->m_ResizeCallback = resizeCallback;
    }

    void Window::SetKeyCallback(Window* window, KeyCallbackFnPt keyCallback) {
        if (window == nullptr) {
            Log::Warning("Tried to set window key callback of nullptr.");
            return;
        }

        window->m_KeyCallback = keyCallback;
    }

    void Window::SetMouseButtonCallback(Window* window, MouseButtonCallbackFnPt mouseCallback) {
        if (window == nullptr) {
            Log::Warning("Tried to set window mouse button callback of nullptr.");
            return;
        }

        window->m_MouseButtonCallback = mouseCallback;
    }

    void Window::SetCursorPosCallback(Window* window, CursorCallbackFnPt cursorCallback) {
        if (window == nullptr) {
            Log::Warning("Tried to set window cursor callback of nullptr.");
            return;
        }

        window->m_CursorCallback = cursorCallback;
    }

    void Window::SetScrollCallback(Window* window, ScrollCallbackFnPt scrollCallback) {
        if (window == nullptr) {
            Log::Warning("Tried to set window scroll callback of nullptr.");
            return;
        }

        window->m_ScrollCallback = scrollCallback;
    }

    void Window::SetWindowCloseCallback(Window* window, WindowCloseCallbackFnPt closeCallback) {
        if (window == nullptr) {
            Log::Warning("Tried to set window close callback of nullptr.");
            return;
        }

        window->m_CloseCallback = closeCallback;
    }

    void Window::SetWindowUserPointer(Window* window, void* userPointer) {
        if (window == nullptr) {
            Log::Warning("Tried to set window user pointer of nullptr.");
            return;
        }

        window->m_WindowUserPointer = userPointer;
    }

    void* Window::GetWindowUserPointer(Window* window) {
        return window->m_WindowUserPointer;
    }
}

#endif