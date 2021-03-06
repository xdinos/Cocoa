#pragma once
#include "externalLibs.h"

namespace Cocoa
{
    class Entity;

    struct Transform
    {
        Transform()
        {
            Init(glm::vec3(0), glm::vec3(1), glm::vec3(0));
        }

        Transform(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation, const char* name = "New GameObject")
        {
            Init(position, scale, eulerRotation, name);
        }

        void UpdateMatrices()
        {
            m_ModelMatrix = glm::translate(glm::mat4(1.0f), m_Position);
            m_ModelMatrix = m_ModelMatrix * glm::toMat4(m_Orientation);
            m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);
        }

        static void Serialize(json& j, Entity entity, const Transform& transform);
        static void Deserialize(json& j, Entity entity);

        glm::vec3 m_Position;
        glm::vec3 m_Scale;
        glm::vec3 m_EulerRotation;
        glm::quat m_Orientation;

        glm::vec3 m_Forward;
        glm::vec3 m_Up;
        glm::vec3 m_Right;

        glm::mat4 m_ModelMatrix;
        glm::mat4 m_InverseModelMatrix;
        const char* m_Name;

    private:
        void Init(glm::vec3 position, glm::vec3 scale, glm::vec3 eulerRotation, const char* name = "New GameObject")
        {
            m_Position = position;
            m_Scale = scale;
            m_EulerRotation = eulerRotation;
            m_Orientation = glm::toQuat(glm::orientate3(m_EulerRotation));

            m_Forward = glm::vec3(0, 0, 1) * m_Orientation;
            m_Up = glm::vec3(0, 1, 0) * m_Orientation;
            m_Right = glm::vec3(1, 0, 0) * m_Orientation;
            m_Name = name;

            this->UpdateMatrices();
        }
    };
}