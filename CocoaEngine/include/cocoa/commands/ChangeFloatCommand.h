#pragma once

#include "cocoa/commands/ICommand.h"
#include "cocoa/components/components.h"

namespace Cocoa
{
    class COCOA ChangeFloatCommand : public ICommand
    {
    public:
        ChangeFloatCommand(float& originalFloat, float newFloat)
            : m_Float(originalFloat), m_NewFloat(newFloat), m_OldFloat(0)
        {
        }

        virtual void execute() override
        {
            m_OldFloat = m_Float;
            m_Float = m_NewFloat;
        }

        virtual void undo() override
        {
            m_Float = m_OldFloat;
        }

        virtual bool mergeWith(ICommand* other) override
        {
            ChangeFloatCommand* changeFloatCommand = dynamic_cast<ChangeFloatCommand*>(other);
            if (changeFloatCommand != nullptr)
            {
                if (&changeFloatCommand->m_Float == &this->m_Float)
                {
                    changeFloatCommand->m_NewFloat = this->m_NewFloat;
                    return true;
                }
            }

            return false;
        }


    private:
        float& m_Float;
        float m_NewFloat;
        float m_OldFloat;
    };
}