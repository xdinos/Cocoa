#pragma once
#include "externalLibs.h"
#include "cocoa/components/components.h"
#include "cocoa/components/Transform.h"
#include "cocoa/systems/System.h"
#include "cocoa/renderer/TextureHandle.h"

namespace Cocoa
{
    // Gizmo Components
    enum class GizmoMode
    {
        None,
        Translate,
        Rotate,
        Scale
    };

    enum class GizmoType
    {
        None,
        Vertical,
        Horizontal,
        Free
    };

    class Gizmo
    {
    public:
        Gizmo(const Sprite& sprite, glm::vec3 offset, float spriteRotation, GizmoType type, glm::vec3 tint={0.4f, 0.4f, 0.4f});

        inline bool GizmoIsActive() { return m_Active; }
        void Render(Camera* camera);
        void GizmoManipulateTranslate(Transform& transform, const glm::vec3& originalDragClickPos, const glm::vec3& mouseOffset, Camera* camera);
        void GizmoManipulateRotate(Transform& transform, const glm::vec3& startPos, const glm::vec3& mouseOffset, Camera* camera);
        void GizmoManipulateScale(Transform& transform, const glm::vec3& originalDragClickPos, const glm::vec3& originalScale, Camera* camera);

    public:
        glm::vec3 m_Position;
        glm::vec3 m_Offset;
        glm::vec3 m_Tint;
        glm::vec2 m_HalfSize;
        glm::vec2 m_TexCoordMin;
        glm::vec2 m_TexCoordMax;

        GizmoType m_Type;
        Box2D m_Box2D;
        uint32 m_TextureAssetId;
        float m_SpriteRotation;
        bool m_Active;
    };

    class GizmoSystem : public System
    {
    public:
        ~GizmoSystem() {}
        GizmoSystem(const char* name="Gizmo System", Scene* scene=nullptr);

        virtual void EditorUpdate(float dt) override;
        virtual void ImGui();
        virtual void OnEvent(Event& e) override;

    private:
        bool HandleKeyPress(KeyPressedEvent& e);
        bool HandleKeyRelease(KeyReleasedEvent& e);
        bool HandleMouseButtonPressed(MouseButtonPressedEvent& e);
        bool HandleMouseButtonReleased(MouseButtonReleasedEvent& e);
        bool HandleMouseScroll(MouseScrolledEvent& e);

    private:
        TextureHandle m_Texture = TextureHandle::null;
        std::unique_ptr<Spritesheet> m_Spritesheet = nullptr;

        bool m_MouseDragging = false;
        int m_ActiveGizmo = -1;
        int m_HotGizmo = -1;
        GizmoMode m_Mode = GizmoMode::Translate;

        glm::vec3 m_MouseOffset;
        glm::vec3 m_OriginalScale;

        glm::vec3 m_OriginalDragClickPos;
        Camera* m_Camera;

        union
        {
            Gizmo m_Gizmos[6];
            struct
            {
                Gizmo m_VtScale;
                Gizmo m_HzScale;
                Gizmo m_FreeScale;

                Gizmo m_VtMove;
                Gizmo m_HzMove;
                Gizmo m_FreeMove;

                //Gizmo m_RotatePositive;
                //Gizmo m_RotateNegative;
                //Gizmo m_RotateFree;
            };
        };
    };
}