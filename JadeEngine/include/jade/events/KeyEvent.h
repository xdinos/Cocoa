#pragma once

#include "jade/events/Event.h"

namespace Jade {
    // ============================================================
    // Base
    // ============================================================
    class JADE KeyEvent : public Event {
    public:
        // TODO: Make this inline somehow
        int GetKeyCode() const;

        EVENT_CLASS_CATEGORY_HEADER(EventCategoryKeyboard | EventCategoryInput)

    protected:
        // Abstract class, should not be instantiated
        KeyEvent(int keycode);

        int m_KeyCode;
    };

    // ============================================================
    // KeyPressed
    // ============================================================
    class JADE KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(int keycode, int repeatCount);

        inline int GetRepeatCount() const;

        virtual std::string ToString() const override;

        EVENT_CLASS_TYPE_HEADER(KeyPressed)

    private:
        int m_RepeatCount;
    };

    // ============================================================
    // KeyReleased
    // ============================================================
    class JADE KeyReleasedEvent : public KeyEvent {
    public:
        KeyReleasedEvent(int keycode);

        virtual std::string ToString() const override;

        EVENT_CLASS_TYPE_HEADER(KeyReleased);
    };

    // ============================================================
    // KeyTyped
    // ============================================================
    class JADE KeyTypedEvent : public KeyEvent {
        KeyTypedEvent(int keycode);

        virtual std::string ToString() const override;

        EVENT_CLASS_TYPE_HEADER(KeyTyped);
    };
}