#include "Gizmos.h"

#include "jade/physics2d/Physics2DSystem.h"
#include "jade/renderer/DebugDraw.h"
#include "jade/renderer/Camera.h"
#include "jade/core/Application.h"
#include "jade/core/Core.h"
#include "jade/util/JMath.h"
#include "jade/commands/ICommand.h"

namespace Jade
{
	// =================================================================================================
	// Gizmo
	// =================================================================================================
	Gizmo::Gizmo(Sprite* sprite, glm::vec3 offset, float spriteRotation, GizmoType type, glm::vec3 darkTint)
	{
		m_Active = false;
		m_Position = glm::vec3();
		m_Box2D = { glm::vec2(10, 40), glm::vec2(5, 20), glm::vec2(0, 0) };
		m_HalfSize = { 8, 20 };

		m_Offset = offset;
		m_SpriteRotation = spriteRotation;
		m_TexCoordMin = sprite->m_TexCoords[0];
		m_TexCoordMax = sprite->m_TexCoords[2];
		m_Texture = sprite->m_Texture;
		m_Tint = darkTint;
		m_Type = type;
	}

	void Gizmo::Render()
	{
		if (m_Active)
		{
			DebugDraw::AddSprite(m_Texture, m_HalfSize * 2.0f, JMath::Vector2From3(m_Position), m_Tint, m_TexCoordMin, m_TexCoordMax, m_SpriteRotation);
		}
		else
		{
			DebugDraw::AddSprite(m_Texture, m_HalfSize * 2.0f, JMath::Vector2From3(m_Position), { 1.0f, 1.0f, 1.0f }, m_TexCoordMin, m_TexCoordMax, m_SpriteRotation);
		}
	}

	void Gizmo::GizmoManipulateTranslate(Transform& transform, const glm::vec3& startPos, const glm::vec3& mouseOffset)
	{
		if (m_Type == GizmoType::Vertical)
		{
			Camera* camera = Application::Get()->GetScene()->GetCamera();
			glm::vec3 mousePosWorld = JMath::Vector3From2(camera->ScreenToOrtho());
			glm::vec3 startToMouse = mousePosWorld - startPos;
			glm::vec3 newPos = glm::vec3(0, startToMouse.y, 0) + startPos - mouseOffset;
			CommandHistory::AddCommand(new ChangeVec3Command(transform.m_Position, newPos));
		}
		else if (m_Type == GizmoType::Horizontal)
		{
			Camera* camera = Application::Get()->GetScene()->GetCamera();
			glm::vec3 mousePosWorld = JMath::Vector3From2(camera->ScreenToOrtho());
			glm::vec3 startToMouse = mousePosWorld - startPos;
			glm::vec3 newPos = glm::vec3(startToMouse.x, 0, 0) + startPos - mouseOffset;
			CommandHistory::AddCommand(new ChangeVec3Command(transform.m_Position, newPos));
		}
		else if (m_Type == GizmoType::Free)
		{
			Camera* camera = Application::Get()->GetScene()->GetCamera();
			glm::vec3 mousePosWorld = JMath::Vector3From2(camera->ScreenToOrtho());
			glm::vec3 startToMouse = mousePosWorld - startPos;
			glm::vec3 newPos = startToMouse + startPos - mouseOffset;
			CommandHistory::AddCommand(new ChangeVec3Command(transform.m_Position, newPos));
		}
	}

	void Gizmo::GizmoManipulateRotate(Transform& transform, const glm::vec3& startPos, const glm::vec3& mouseOffset)
	{

	}

	void Gizmo::GizmoManipulateScale(Transform& transform, const glm::vec3& startPos, const glm::vec3& mouseOffset)
	{

	}

	// =================================================================================================
	// Gizmo Controller
	// =================================================================================================
	GizmoSystem::GizmoSystem(const char* name)
		: System(name)
	{
		m_Texture = std::unique_ptr<Texture>(new Texture("assets/images/gizmos.png", false));
		m_Spritesheet = std::unique_ptr<Spritesheet>(new Spritesheet(m_Texture.get(), 16, 40, 9, 0));

		float hzOffsetX = 15;
		float hzOffsetY = -10;
		float squareOffsetX = 5;
		float squareOffsetY = 15;
		float vtOffsetX = -8;
		float vtOffsetY = 12;
		m_HzMove = Gizmo(&m_Spritesheet->GetSprite(1), { hzOffsetX, hzOffsetY, 0.0f}, -90.0f, GizmoType::Horizontal);
		m_VtMove = Gizmo(&m_Spritesheet->GetSprite(4), { vtOffsetX, vtOffsetY, 0.0f }, 0.0f, GizmoType::Vertical);
		m_FreeMove = Gizmo(&m_Spritesheet->GetSprite(0), { squareOffsetX, squareOffsetY, 0.0f }, 0.0f, GizmoType::Free);
		m_FreeMove.m_Box2D = { glm::vec2(16, 16), glm::vec2(8, 8), glm::vec2(0, -12) };

		m_HzScale = Gizmo(&m_Spritesheet->GetSprite(2), { hzOffsetX, hzOffsetY, 0.0f }, 90.0f, GizmoType::Horizontal);
		m_VtScale = Gizmo(&m_Spritesheet->GetSprite(5), { vtOffsetX, vtOffsetY, 0.0f }, 180.0f, GizmoType::Vertical);
		m_FreeScale = Gizmo(&m_Spritesheet->GetSprite(1), { squareOffsetX, squareOffsetY, 0.0f }, 0.0f, GizmoType::Free);
	}

	void GizmoSystem::ImGui(entt::registry& registry)
	{
		//ImGui::Begin("Gizmo debug");
		//ImGui::DragFloat2("Free Move Offset:", &m_FreeMove.m_Offset[0]);
		//ImGui::DragFloat2("Free Move HalfSize: ", &m_FreeMove.m_HalfSize[0]);
		//ImGui::DragFloat2("Free Move BBHalfSize: ", &m_FreeMove.m_Box2D.m_HalfSize[0]);

		//ImGui::DragFloat2("HzMove Offset:", &m_HzMove.m_Offset[0]);
		//ImGui::DragFloat2("HzMove HalfSize: ", &m_HzMove.m_HalfSize[0]);
		//ImGui::DragFloat2("HzMove BBHalfSize: ", &m_HzMove.m_Box2D.m_HalfSize[0]);

		//ImGui::DragFloat2("VtMove Offset:", &m_VtMove.m_Offset[0]);
		//ImGui::DragFloat2("VtMove HalfSize: ", &m_VtMove.m_HalfSize[0]);
		//ImGui::DragFloat2("VtMove BBHalfSize: ", &m_VtMove.m_Box2D.m_HalfSize[0]);
		//ImGui::End();
	}

	void GizmoSystem::Update(entt::registry& registry, float dt)
	{
		entt::entity activeEntity = Application::Get()->GetScene()->GetActiveEntity();
		if (entt::to_integral(activeEntity) != entt::to_integral(entt::null))
		{
			Transform& entityTransform = registry.get<Transform>(activeEntity);

			if (m_MouseDragging)
			{
				switch (m_Mode)
				{
					case GizmoMode::Translate:
						m_Gizmos[m_ActiveGizmo].GizmoManipulateTranslate(entityTransform, m_GizmoStartPos, m_MouseOffset);
						break;
					case GizmoMode::Rotate:
						m_Gizmos[m_ActiveGizmo].GizmoManipulateRotate(entityTransform, m_GizmoStartPos, m_MouseOffset);
						break;
					case GizmoMode::Scale:
						m_Gizmos[m_ActiveGizmo].GizmoManipulateScale(entityTransform, m_GizmoStartPos, m_MouseOffset);
						break;
				}
			}

			int start = 0;
			int end = 3;
			if (m_Mode == GizmoMode::Translate)
			{
				start = 3;
				end = 6;
			}

			Camera* camera = Application::Get()->GetScene()->GetCamera();
			glm::vec2 mousePosWorld = camera->ScreenToOrtho();
			bool anyHot = false;
			for (int i = start; i < end; i++)
			{
				Gizmo gizmo = m_Gizmos[i];
				gizmo.m_Position = entityTransform.m_Position + gizmo.m_Offset;
				glm::vec3 boxPos = gizmo.m_Position + JMath::Vector3From2(gizmo.m_Box2D.m_Offset);
				if (!m_MouseDragging && Physics2D::PointInBox(mousePosWorld, gizmo.m_Box2D.m_HalfSize, boxPos, gizmo.m_SpriteRotation))
				{
					gizmo.m_Active = true;
					m_HotGizmo = i;
					anyHot = true;
				}
				gizmo.Render();
			}
			if (!anyHot)
			{
				m_HotGizmo = -1;
			}
		}
	}

	void GizmoSystem::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(std::bind(&GizmoSystem::HandleKeyPress, this, std::placeholders::_1));
		dispatcher.Dispatch<KeyReleasedEvent>(std::bind(&GizmoSystem::HandleKeyRelease, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseButtonPressedEvent>(std::bind(&GizmoSystem::HandleMouseButtonPressed, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(std::bind(&GizmoSystem::HandleMouseButtonReleased, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseScrolledEvent>(std::bind(&GizmoSystem::HandleMouseScroll, this, std::placeholders::_1));
	}

	bool GizmoSystem::HandleKeyPress(KeyPressedEvent& e)
	{

		return false;
	}

	bool GizmoSystem::HandleKeyRelease(KeyReleasedEvent& e)
	{

		return false;
	}

	bool GizmoSystem::HandleMouseScroll(MouseScrolledEvent& e)
	{

		return false;
	}

	bool GizmoSystem::HandleMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (!m_MouseDragging && e.GetMouseButton() == JADE_MOUSE_BUTTON_LEFT)
		{
			Camera* camera = Application::Get()->GetScene()->GetCamera();
			glm::vec2 mousePosWorld = camera->ScreenToOrtho();
			std::pair<entt::registry&, entt::entity> result = Physics2D::OverlapPoint(mousePosWorld);
			if (entt::to_integral(result.second) != entt::to_integral(entt::null))
			{
				Application::Get()->GetScene()->SetActiveEntity(result.second);
				const Transform& transform = result.first.get<Transform>(result.second);
				m_ActiveGizmo = m_HotGizmo;
				m_MouseDragging = true;
				m_GizmoStartPos = JMath::Vector3From2(mousePosWorld);
				m_MouseOffset = JMath::Vector3From2(mousePosWorld) - transform.m_Position;
			}
			else if (m_HotGizmo != -1)
			{
				entt::entity activeEntity = Application::Get()->GetScene()->GetActiveEntity();
				const Transform& transform = Application::Get()->GetScene()->GetRegistry().get<Transform>(activeEntity);
				m_ActiveGizmo = m_HotGizmo;
				m_MouseDragging = true;
				m_GizmoStartPos = JMath::Vector3From2(mousePosWorld);
				m_MouseOffset = JMath::Vector3From2(mousePosWorld) - transform.m_Position;
			}
			else
			{
				m_ActiveGizmo = -1;
				Application::Get()->GetScene()->SetActiveEntity(result.second);
			}
		}

		return false;
	}

	bool GizmoSystem::HandleMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		if (m_MouseDragging && e.GetMouseButton() == JADE_MOUSE_BUTTON_LEFT)
		{
			m_MouseDragging = false;
			CommandHistory::SetNoMergeMostRecent();
		}
		return false;
	}
}