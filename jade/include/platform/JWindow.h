#pragma once

#include "core/Core.h"
#include "scenes/Scene.h"
#include "util/Log.h"
#include "core/ImGuiLayer.h"
#include "renderer/Framebuffer.h"
#include "renderer/DebugDraw.h"

class JWindow {
public:
    virtual void Destroy() = 0;

    static void KeyCallback(int key, int scancode, int action, int mods) {
        JWindow* win = Get();
        for (int i=0; i < 3; i++) {
            if (win->keyCallbacks[i] != nullptr) {
                win->keyCallbacks[i](key, scancode, action, mods);
            }
        }
    }

    static void CursorCallback(double xpos, double ypos) {
        JWindow* win = Get();
        for (int i=0; i < 3; i++) {
            if (win->cursorCallbacks[i] != nullptr) {
                win->cursorCallbacks[i](xpos, ypos);
            }
        }
    }

    static void MouseButtonCallback(int button, int action, int mods) {
        JWindow* win = Get();
        for (int i=0; i < 3; i++) {
            if (win->mouseButtonCallbacks[i] != nullptr) {
                win->mouseButtonCallbacks[i](button, action, mods);
            }
        }
    }
    static void ScrollCallback(double xoffset, double yoffset) {
        JWindow* win = Get();
        for (int i=0; i < 3; i++) {
            if (win->scrollCallbacks[i] != nullptr) {
                win->scrollCallbacks[i](xoffset, yoffset);
            }
        }
    }
    static void ResizeCallback(int width, int height) {
        JWindow* win = Get();
        for (int i=0; i < 3; i++) {
            if (win->resizeCallbacks[i] != nullptr) {
                win->resizeCallbacks[i](width, height);
            }
        }
    }

    static void RegisterKeyCallback(KeyCallbackFnPt callback) {
        JWindow* win = Get();
        bool assigned = false;
        for (int i=0; i < 3; i++) {
            if (win->keyCallbacks[i] == nullptr) {
                win->keyCallbacks[i] = callback;
                assigned = true;
                break;
            }
        }

        if (!assigned) {
            Log::Warning("Cannot register callback. Maximum number of key callbacks is 3.");
        }
    }
    static void RegisterCursorCallback(CursorCallbackFnPt callback) {
        JWindow* win = Get();
        bool assigned = false;
        for (int i=0; i < 3; i++) {
            if (win->cursorCallbacks[i] == nullptr) {
                win->cursorCallbacks[i] = callback;
                assigned = true;
                break;
            }
        }

        if (!assigned) {
            Log::Warning("Cannot register callback. Maximum number of key callbacks is 3.");
        }
    }
    static void RegisterMouseButtonCallback(MouseButtonCallbackFnPt callback) {
        JWindow* win = Get();
        bool assigned = false;
        for (int i=0; i < 3; i++) {
            if (win->mouseButtonCallbacks[i] == nullptr) {
                win->mouseButtonCallbacks[i] = callback;
                assigned = true;
                break;
            }
        }

        if (!assigned) {
            Log::Warning("Cannot register callback. Maximum number of key callbacks is 3.");
        }
    }
    static void RegisterScrollCallback(ScrollCallbackFnPt callback) {
        JWindow* win = Get();
        bool assigned = false;
        for (int i=0; i < 3; i++) {
            if (win->scrollCallbacks[i] == nullptr) {
                win->scrollCallbacks[i] = callback;
                assigned = true;
                break;
            }
        }

        if (!assigned) {
            Log::Warning("Cannot register callback. Maximum number of key callbacks is 3.");
        }
    }
    static void RegisterResizeCallback(ResizeCallbackFnPt callback) {
        JWindow* win = Get();
        bool assigned = false;
        for (int i=0; i < 3; i++) {
            if (win->resizeCallbacks[i] == nullptr) {
                win->resizeCallbacks[i] = callback;
                assigned = true;
                break;
            }
        }

        if (!assigned) {
            Log::Warning("Cannot register callback. Maximum number of key callbacks is 3.");
        }
    }

    static void Hide() { JWindow::Get()->_Hide(); }
    static void Close() { JWindow::Get()->_Close(); }
    static void Stop() { JWindow::Get()->_Stop(); }
    static bool IsRunning() { return JWindow::Get()->_IsRunning(); }

    static void SetWidth(int newWidth) { JWindow::Get()->m_Width = newWidth; }
    static void SetHeight(int newHeight) { JWindow::Get()->m_Height = newHeight; }

    static int GetWidth() { return JWindow::Get()->m_Width; }
    static int GetHeight() { return JWindow::Get()->m_Height; }
    static float GetTargetAspectRatio() { return JWindow::Get()->m_TargetAspectRatio; }
    static Framebuffer* GetFramebuffer() { return JWindow::Get()->m_Framebuffer; }
    static Scene* GetScene() { return JWindow::Get()->m_CurrentScene; }
    static void* GetWindowHandle() { return JWindow::Get()->m_WindowHandle; }

    static void Update(JWindow* window, float dt) { 
        DebugDraw::BeginFrame();
        window->m_CurrentScene->Update(dt); 
    }
    static void Render() { JWindow::Get()->_Render(); }

    static void ChangeScene(Scene* newScene) {
        JWindow* win = JWindow::Get();
        win->m_CurrentScene = newScene;
        win->m_CurrentScene->Init();
        win->m_CurrentScene->Start();
    }

    static JWindow* Get();

private:
    void Init() {
        for (int i=0; i < 3; i++) {
            keyCallbacks[i] = nullptr;
            mouseButtonCallbacks[i] = nullptr;
            scrollCallbacks[i] = nullptr;
            cursorCallbacks[i] = nullptr;
            resizeCallbacks[i] = nullptr;
        }
    }

protected:
    virtual JWindow* _Create() = 0;
    virtual void _Hide() = 0;
    virtual void _Close() = 0;
    virtual void _Stop() = 0;
    virtual void _Render() = 0;

    const bool _IsRunning() { return m_Running; }

protected:
    static JWindow* m_Instance;
    void* m_WindowHandle;

    // Registered callbacks
    KeyCallbackFnPt           keyCallbacks[3];
    CursorCallbackFnPt        cursorCallbacks[3];
    MouseButtonCallbackFnPt   mouseButtonCallbacks[3];
    ScrollCallbackFnPt        scrollCallbacks[3];
    ResizeCallbackFnPt        resizeCallbacks[3];

    Scene* m_CurrentScene;
    Framebuffer* m_Framebuffer;

    bool m_Running = true;
    int m_Width = 1920;
    int m_Height = 1080;
    float m_TargetAspectRatio = 3840.0f / 2160.0f;
};


#ifdef _JADE_PLATFORM_WINDOWS
// Put all different implementation stubs in here, so that we can get rid
// of circular dependencies
#include "platform/windows/Win32Window.h"
#endif
