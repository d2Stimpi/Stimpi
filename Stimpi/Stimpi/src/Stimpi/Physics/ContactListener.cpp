#include "stpch.h"
#include "Stimpi/Physics/ContactListener.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Physics/Physics.h"

#include "Stimpi/Core/EventQueue.h"

#include "box2d/b2_contact.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_settings.h"

#define CONTACTLISTENER_DBG	(false)

namespace Stimpi
{

	static std::string GetStringCollisionEventType(CollisionEventType e)
	{
		switch (e)
		{
		case CollisionEventType::COLLISION_BEGIN:		return std::string("COLLISION_BEGIN");
		case CollisionEventType::COLLISION_END:			return std::string("COLLISION_END");
		case CollisionEventType::COLLISION_PRESOLVE:	return std::string("COLLISION_PRESOLVE");
		case CollisionEventType::COLLISION_POSTSOLVE:	return std::string("COLLISION_POSTSOLVE");
		default: return std::string("NONE");
		}
	}

	bool ContactListener::InvokeCollisionEventMethod(CollisionEventType type, Collision collision)
	{
		Entity owner = { (entt::entity)collision.m_Owner, m_Scene };
		if (owner.HasComponent<ScriptComponent>())
		{
			auto instance = ScriptEngine::GetScriptInstance(owner);
			if (instance == nullptr)
			{
				//ST_CORE_INFO("Skipp processing physics event, no Script instance! OwnerID: {}", (uint32_t)owner);
				//ST_CORE_ASSERT_MSG(!instance, "Processing physics event, but no Script instance! Owner: {}", (uint32_t)owner);
				return true;
			}

			switch (type)
			{
			case CollisionEventType::COLLISION_BEGIN:
				instance->InvokeOnCollisionBegin(collision);
				break;
			case CollisionEventType::COLLISION_END:
				instance->InvokeOnCollisionEnd(collision);
				break;
			case CollisionEventType::COLLISION_PRESOLVE:
				return instance->InvokeOnCollisionPreSolve(collision);
				break;
			case CollisionEventType::COLLISION_POSTSOLVE:
				instance->InvokeOnCollisionPostSolve(collision);
				break;
			}
		}

		// TODO: check if there is need to do something more onBegin, onEnd, onPostResolve
		return true;
	}

	void ContactListener::PopulateCollisionEventData(b2Contact* contact, Collision* collision)
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

			collision->m_Contacts.push_back(ct);
			if (CONTACTLISTENER_DBG) ST_CORE_INFO("ContactListener: contact point {}", ct.m_Point);
		}

		b2Vec2 vel1 = bodyA->GetLinearVelocityFromLocalPoint(worldManifold.points[0]);	// TODO: what about [1] point?
		b2Vec2 vel2 = bodyB->GetLinearVelocityFromLocalPoint(worldManifold.points[0]);
		b2Vec2 impactVelocity = vel1 - vel2;
		collision->m_ImpactVelocity = { impactVelocity.x, impactVelocity.y };
	}

	bool ContactListener::EmitCollisionEvents(CollisionEventType type, b2Contact* contact)
	{
		bool resultA = true;
		bool resultB = true;

		b2Body* bodyA = contact->GetFixtureA()->GetBody();
		b2Body* bodyB = contact->GetFixtureB()->GetBody();
		b2BodyUserData userDataA = bodyA->GetUserData();
		b2BodyUserData userDataB = bodyB->GetUserData();

		std::shared_ptr<Collision> collisionA = std::make_shared<Collision>();
		collisionA->m_Owner = userDataA.pointer;
		collisionA->m_ColliderEntityID = userDataB.pointer;
		PopulateCollisionEventData(contact, collisionA.get());

		// For debug contact draw
		if (type == CollisionEventType::COLLISION_BEGIN)
		{
			Physics::AddActiveCollision(collisionA);
		}
		else if (type == CollisionEventType::COLLISION_POSTSOLVE || type == CollisionEventType::COLLISION_PRESOLVE)
		{
			Physics::UpdateActiveCollision(collisionA.get());
		}
		else if (type == CollisionEventType::COLLISION_END)
		{
			Physics::RemoveActiveCollision(collisionA.get());
		}


		Physics::SetActiveCollision(collisionA);
		// Invoke script
		resultA = (InvokeCollisionEventMethod(type, *collisionA));

		std::shared_ptr<Collision> collisionB = std::make_shared<Collision>();
		collisionB->m_Owner = userDataB.pointer;
		collisionB->m_ColliderEntityID = userDataA.pointer;
		PopulateCollisionEventData(contact, collisionB.get());

		Physics::SetActiveCollision(collisionB);
		// Invoke script
		resultB = InvokeCollisionEventMethod(type, *collisionB);

		if (CONTACTLISTENER_DBG) ST_CORE_INFO("ContactListener: Event {} - A: {}, B: {}", GetStringCollisionEventType(type), userDataA.pointer, userDataB.pointer);
		
		// If either PreProcess returned false, return false
		if (resultA == false || resultB == false)
			return false;
		else
			return true;
	}

	void ContactListener::SetContext(Scene* scene)
	{
		m_Scene = scene;
	}

	void ContactListener::BeginContact(b2Contact* contact)
	{
		EmitCollisionEvents(CollisionEventType::COLLISION_BEGIN, contact);
	}

	void ContactListener::EndContact(b2Contact* contact)
	{
		EmitCollisionEvents(CollisionEventType::COLLISION_END, contact);
	}

	void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{
		bool contactEnabled = true;
		contactEnabled = EmitCollisionEvents(CollisionEventType::COLLISION_PRESOLVE, contact);

		//ST_CORE_INFO("PreSolve - SetEnabled: {}", contactEnabled);
		contact->SetEnabled(contactEnabled);
	}

	void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
	{
		EmitCollisionEvents(CollisionEventType::COLLISION_POSTSOLVE, contact);
		//ST_CORE_INFO("PostSolve - IsEnabled: {}", contact->IsEnabled());
	}

}