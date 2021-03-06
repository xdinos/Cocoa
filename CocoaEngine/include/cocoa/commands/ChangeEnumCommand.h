#pragma once

#include "cocoa/commands/ICommand.h"
#include "cocoa/components/components.h"

namespace Cocoa
{
    template<typename T>
    class ChangeEnumCommand : public ICommand
    {
    public:
        ChangeEnumCommand(T& originalEnum, T newEnum)
            : m_Enum(originalEnum), m_NewEnum(newEnum), m_OldEnum(static_cast<T>(0))
        {
        }

        virtual void execute() override
        {
            m_OldEnum = m_Enum;
            m_Enum = m_NewEnum;
        }

        virtual void undo() override
        {
            m_Enum = m_OldEnum;
        }

        virtual bool mergeWith(ICommand* other) override
        {
            ChangeEnumCommand<T>* changeEnumCommand = dynamic_cast<ChangeEnumCommand<T>*>(other);
            if (changeEnumCommand != nullptr)
            {
                if (&changeEnumCommand->m_Enum == &this->m_Enum)
                {
                    changeEnumCommand->m_NewEnum = this->m_NewEnum;
                    return true;
                }
            }

            return false;
        }


    private:
        T& m_Enum;
        T m_NewEnum;
        T m_OldEnum;
    };
}