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

#define CONTACTLISTENER_DBG	(false)

namespace Stimpi
{
	static void PopulateCollisionEventData(b2Contact* contact, Collision& collision)
	{
		b2Body* bodyA = contact->GetFixtureA()->GetBody();
		b2Body* bodyB = contact->GetFixtureB()->GetBody();

		b2WorldManifold worldManifold;
		contact->GetWorldManifold(&worldManifold);

		uint32_t numPoints = contact->GetManifold()->pointCount;
		for (uint32_t i = 0; i < numPoints; i++)
		{
			Contact ct;
			ct.m_Point = { worldManifold.points[i].x, worldManifold.points[i].y };
			
			b2Vec2 vel1 = bodyA->GetLinearVelocityFromLocalPoint(worldManifold.points[i]);
			b2Vec2 vel2 = bodyB->GetLinearVelocityFromLocalPoint(worldManifold.points[i]);
			b2Vec2 impactVelocity = vel1 - vel2;
			ct.m_ImpactVelocity = { impactVelocity.x, impactVelocity.y };

			collision.m_Contacts.push_back(ct);
			if (CONTACTLISTENER_DBG) ST_CORE_INFO("ContactListener: contact point {}", ct.m_Point);
		}

		b2Vec2 vel1 = bodyA->GetLinearVelocityFromLocalPoint(worldManifold.points[0]);	// TODO: what about [1] point?
		b2Vec2 vel2 = bodyB->GetLinearVelocityFromLocalPoint(worldManifold.points[0]);
		b2Vec2 impactVelocity = vel1 - vel2;
		collision.m_ImpactVelocity = { impactVelocity.x, impactVelocity.y };
	}

	static void EmitCollisionEvents(PhysicsEventType type, b2Contact* contact)
	{
		b2Body* bodyA = contact->GetFixtureA()->GetBody();
		b2Body* bodyB = contact->GetFixtureB()->GetBody();
		b2BodyUserData userDataA = bodyA->GetUserData();
		b2BodyUserData userDataB = bodyB->GetUserData();

		Collision collisionA = Collision();
		collisionA.m_Owner = userDataA.pointer;
		collisionA.m_ColliderEntityID = userDataB.pointer;
		PopulateCollisionEventData(contact, collisionA);

		auto eventA = std::make_shared<PhysicsEvent>();
		eventA.reset(PhysicsEvent::CreatePhysicsEvent(type, collisionA));
		EventQueue<PhysicsEvent>::PushEvent(eventA);

		Collision collisionB = Collision();
		collisionB.m_Owner = userDataB.pointer;
		collisionB.m_ColliderEntityID = userDataA.pointer;
		PopulateCollisionEventData(contact, collisionB);

		auto eventB = std::make_shared<PhysicsEvent>();
		eventB.reset(PhysicsEvent::CreatePhysicsEvent(type, collisionB));
		EventQueue<PhysicsEvent>::PushEvent(eventB);

		if (CONTACTLISTENER_DBG) ST_CORE_INFO("ContactListener: Event {} - A: {}, B: {}", GetStringPhysicsEvent(type), userDataA.pointer, userDataB.pointer);
	}

	void ContactListener::SetContext(Scene* scene)
	{
		m_Scene = scene;
	}

	void ContactListener::BeginContact(b2Contact* contact)
	{
		EmitCollisionEvents(PhysicsEventType::COLLISION_BEGIN, contact);
	}

	void ContactListener::EndContact(b2Contact* contact)
	{
		EmitCollisionEvents(PhysicsEventType::COLLISION_END, contact);
	}

	void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{
		EmitCollisionEvents(PhysicsEventType::COLLISION_PRESOLVE, contact);
	}

	void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
	{
		EmitCollisionEvents(PhysicsEventType::COLLISION_POSTSOLVE, contact);
	}

}