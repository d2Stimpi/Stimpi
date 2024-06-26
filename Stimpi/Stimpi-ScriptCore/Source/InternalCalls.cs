﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace Stimpi
{
    public class InternalCalls
    {
        /* Entity & Components */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Entity_HasComponent(uint entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Entity_AddComponent(uint entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Entity_RemoveComponent(uint entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static uint Entity_FindEntityByName(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static object GetScriptInstace(uint entityID);

        /* TagComponent */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void TagComponent_GetString(uint entityID, out string outTag);

        /* QuadComponent */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool QuadComponent_GetPosition(uint entityID, out Vector2 outPosition);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool QuadComponent_SetPosition(uint entityID, ref Vector2 position);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool QuadComponent_GetSize(uint entityID, out Vector2 outSize);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool QuadComponent_SetSize(uint entityID, ref Vector2 size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool QuadComponent_GetRotation(uint entityID, out float outRotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool QuadComponent_SetRotation(uint entityID, ref float rotation);

        /* SpriteComponent */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool SpriteComponent_GetColor(uint entityID, out Color outColor);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool SpriteComponent_SetColor(uint entityID, ref Color color);

        /* AnimatedSpriteComponent */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_IsAnimationSet(uint entityID, out bool isSet);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_AnimStart(uint entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_AnimPause(uint entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_AnimStop(uint entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_GetAnimState(uint entityID, out int outState);

        /* RigidBody2DComponent */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool RigidBody2DComponent_GetRigidBodyType(uint entityID, out int outType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool RigidBody2DComponent_SetRigidBodyType(uint entityID, int type);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool RigidBody2DComponent_GetFixedRotation(uint entityID, out bool outFixedRotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool RigidBody2DComponent_SetFixedRotation(uint entityID, bool fixedRotation);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool RigidBody2DComponent_GetTransform(uint entityID, out Vector2 outPosition, out float outAngle);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool RigidBody2DComponent_SetTransform(uint entityID, ref Vector2 position, ref float angle);

        /* Input */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Input_IsKeyPressed(KeyCode keycode);

        /* Physics */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Physics_ApplyForce(uint entityID, ref Vector2 force, ref Vector2 point, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Physics_ApplyForceCenter(uint entityID, ref Vector2 force, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Physics_ApplyLinearImpulse(uint entityID, ref Vector2 impulse, ref Vector2 point, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Physics_ApplyLinearImpulseCenter(uint entityID, ref Vector2 impulse, bool wake);

        /* Collision */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Collision_GetImpactVelocity(uint OwnerID, uint TargetID, out Vector2 velocity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static Contact[] Collision_GetContacts(uint OwnerID, uint TargetID);
    }
}
