#include "stpch.h"
#include "Stimpi/Physics/ContactListener.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Physics/Physics.h"

#include "Stimpi/Core/EventQueue.h"

#include "box2d/b2_contact.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_settings.h"

namespace Stimpi
{
	void ContactListener::SetContext(Scene* scene)
	{
		m_Scene = scene;
	}

	void ContactListener::BeginContact(b2Contact* contact)
	{
		b2BodyUserData userDataA = contact->GetFixtureA()->GetBody()->GetUserData();
		b2BodyUserData userDataB = contact->GetFixtureB()->GetBody()->GetUserData();
		
		Collision collisionA = Collision();
		collisionA.m_Owner = userDataA.pointer;
		collisionA.m_ColliderEntityID = userDataB.pointer;
		
		// Contact point
		b2Vec2 point = contact->GetManifold()->points[0].localPoint;
		collisionA.m_Point.m_Position = { point.x, point.y };

		auto eventA = std::make_shared<PhysicsEvent>();
		eventA.reset(PhysicsEvent::CreatePhysicsEvent(PhysicsEventType::COLLISION_BEGIN, collisionA));
		EventQueue<PhysicsEvent>::PushEvent(eventA);

		Collision collisionB = Collision();
		collisionB.m_Owner = userDataB.pointer;
		collisionB.m_ColliderEntityID = userDataA.pointer;

		auto eventB = std::make_shared<PhysicsEvent>();
		eventB.reset(PhysicsEvent::CreatePhysicsEvent(PhysicsEventType::COLLISION_BEGIN, collisionB));
		EventQueue<PhysicsEvent>::PushEvent(eventB);

		ST_CORE_INFO("ContactListener: contact Begin between {}, {}", userDataA.pointer, userDataB.pointer);
		ST_CORE_INFO("ContactListener: contact point {}", collisionA.m_Point.m_Position);
	}

	void ContactListener::EndContact(b2Contact* contact)
	{
		b2BodyUserData userDataA = contact->GetFixtureA()->GetBody()->GetUserData();
		b2BodyUserData userDataB = contact->GetFixtureB()->GetBody()->GetUserData();

		Collision collisionA = Collision();
		collisionA.m_Owner = userDataA.pointer;
		collisionA.m_ColliderEntityID = userDataB.pointer;

		auto eventA = std::make_shared<PhysicsEvent>();
		eventA.reset(PhysicsEvent::CreatePhysicsEvent(PhysicsEventType::COLLISION_END, collisionA));
		EventQueue<PhysicsEvent>::PushEvent(eventA);

		Collision collisionB = Collision();
		collisionB.m_Owner = userDataB.pointer;
		collisionB.m_ColliderEntityID = userDataA.pointer;

		auto eventB = std::make_shared<PhysicsEvent>();
		eventB.reset(PhysicsEvent::CreatePhysicsEvent(PhysicsEventType::COLLISION_END, collisionB));
		EventQueue<PhysicsEvent>::PushEvent(eventB);

		ST_CORE_INFO("ContactListener: contact End between {}, {}", userDataA.pointer, userDataB.pointer);
	}

}