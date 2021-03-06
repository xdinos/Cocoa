#include "externalLibs.h"

#include "cocoa/physics2d/Physics2D.h"
#include "cocoa/components/components.h"
#include "cocoa/components/Transform.h"
#include "cocoa/physics2d/rigidbody/CollisionDetector2D.h"
#include "cocoa/core/Application.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/Settings.h"

namespace Cocoa
{
	AABB Physics2D::GetBoundingBoxForPixels(uint8* pixels, int width, int height, int channels)
	{
		AABB result = Physics2DSystem::AABBFrom(glm::vec2{ 0, 0 }, glm::vec2{ width, height }, glm::vec2{ 0, 0 });

		if (channels == 4)
		{
			// Initialize minimum to the texture size, and max to 0
			int minX = width, minY = height;
			int maxX = 0, maxY = 0;

			uint8* pixelOffset = pixels;
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					uint8 alpha = pixelOffset[3];
					// If we find non-transparent pixel, update the box bounds if required
					if (alpha > 10)
					{
						if (minX > x) minX = x;
						if (maxX < x) maxX = x;

						if (minY > y) minY = y;
						if (maxY < y) maxY = y;
					}

					pixelOffset += channels;
				}
			}

			// Prevent off by one error
			maxX++;
			maxY++;

			glm::vec2 center = { width / 2.0f, height / 2.0f };
			glm::vec2 subCenter = { maxX - ((maxX - minX) / 2.0f), maxY - ((maxY - minY) / 2.0f) };

			glm::vec2 offset = subCenter - center;
			offset.y *= -1;
			result = Physics2DSystem::AABBFrom(glm::vec2{ minX, minY }, glm::vec2{ maxX, maxY }, offset);
		}

		return result;
	}

	Entity Physics2D::OverlapPoint(const glm::vec2& point)
	{
		Physics2D* physics = Get();
		auto group = physics->m_Scene->GetRegistry().group<AABB>(entt::get<Transform>);
		for (Entity entity : group)
		{
			auto& aabb = entity.GetComponent<AABB>();
			auto& transform = entity.GetComponent<Transform>();
			if (CollisionDetector2D::PointInAABB(point, aabb))
			{
				return entity;
			}
		};

		return Entity::Null;
	}

	bool Physics2D::PointInBox(const glm::vec2& point, const glm::vec2& halfSize, const glm::vec2& position, float rotationDegrees)
	{
		Physics2D* physics = Get();
		b2PolygonShape shape;
		shape.SetAsBox(halfSize.x, halfSize.y);
		b2Transform transform = b2Transform(b2Vec2(position.x, position.y), b2Rot(CMath::ToRadians(rotationDegrees)));
		return shape.TestPoint(transform, b2Vec2(point.x, point.y));
	}

	void Physics2D::AddEntity(Entity entity)
	{
		if (entity.HasComponent<Transform, Rigidbody2D>())
		{
			Transform& transform = entity.GetComponent<Transform>();
			Rigidbody2D& rb = entity.GetComponent<Rigidbody2D>();

			b2BodyDef bodyDef;
			bodyDef.position.Set(transform.m_Position.x, transform.m_Position.y);
			bodyDef.angle = CMath::ToRadians(transform.m_EulerRotation.z);
			bodyDef.angularDamping = rb.m_AngularDamping;
			bodyDef.linearDamping = rb.m_LinearDamping;
			bodyDef.fixedRotation = rb.m_FixedRotation;
			bodyDef.bullet = rb.m_ContinuousCollision;

			if (rb.m_BodyType == BodyType2D::Dynamic)
			{
				bodyDef.type = b2BodyType::b2_dynamicBody;
			}
			else if (rb.m_BodyType == BodyType2D::Static)
			{
				bodyDef.type = b2BodyType::b2_staticBody;
			}
			else
			{
				bodyDef.type = b2BodyType::b2_kinematicBody;
			}

			b2Body* body = m_World.CreateBody(&bodyDef);
			rb.m_RawRigidbody = body;

			b2PolygonShape shape;
			if (entity.HasComponent<Box2D>())
			{
				Box2D& box = entity.GetComponent<Box2D>();
				shape.SetAsBox(box.m_HalfSize.x * transform.m_Scale.x, box.m_HalfSize.y * transform.m_Scale.y);
				b2Vec2 pos = bodyDef.position;
				bodyDef.position.Set(pos.x - box.m_HalfSize.x * transform.m_Scale.x, pos.y - box.m_HalfSize.y * transform.m_Scale.y);
			}
			else if (entity.HasComponent<Circle>())
			{
				// TODO: IMPLEMENT ME
				Circle& circle = entity.GetComponent<Circle>();
			}

			body->CreateFixture(&shape, rb.m_Mass);
		}
	}

	void Physics2D::Update(float dt)
	{
		m_PhysicsTime += dt;
		while (m_PhysicsTime > 0.0f)
		{
			m_World.Step(Settings::Physics2D::s_Timestep, Settings::Physics2D::s_VelocityIterations, Settings::Physics2D::s_PositionIterations);
			m_PhysicsTime -= Settings::Physics2D::s_Timestep;
		}

		auto view = m_Scene->GetRegistry().view<Transform, Rigidbody2D>();
		for (Entity entity : view)
		{
			Transform& transform = entity.GetComponent<Transform>();
			Rigidbody2D& rb = entity.GetComponent<Rigidbody2D>();
			b2Body* body = static_cast<b2Body*>(rb.m_RawRigidbody);
			b2Vec2 position = body->GetPosition();
			transform.m_Position.x = position.x;
			transform.m_Position.y = position.y;
			transform.m_EulerRotation.z = CMath::ToDegrees(body->GetAngle());
		}
	}

	void Physics2D::Destroy()
	{
		auto view =  m_Scene->GetRegistry().view<Rigidbody2D>();// m_Registry.view<Rigidbody2D>();
		for (Entity entity : view)
		{
			Rigidbody2D& rb = entity.GetComponent<Rigidbody2D>();

			// Manually destroy all bodies, in case the physics system would like
			// to use this world again
			m_World.DestroyBody(static_cast<b2Body*>(rb.m_RawRigidbody));
			rb.m_RawRigidbody = nullptr;
		}
	}

	// -------------------------------------------------------------------------
	// Should be called once at beginning of engine initialization
	// -------------------------------------------------------------------------
	std::unique_ptr<Physics2D> Physics2D::s_Instance = nullptr;
	void Physics2D::Init()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = std::make_unique<Physics2D>(nullptr);
		}
		else
		{
			Log::Error("Tried to reinitialize Physics2D.");
		}
	}

	void Physics2D::SetScene(Scene* scene)
	{
		s_Instance->m_Scene = scene;
	}

	Physics2D* Physics2D::Get()
	{
		Log::Assert(s_Instance != nullptr, "Physics2D was never initialized.");

		return s_Instance.get();
	}

	Physics2D::Physics2D(Scene* scene) 
		: m_Scene(scene) {}
}