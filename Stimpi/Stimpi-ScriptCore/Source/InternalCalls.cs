using System;
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
        public extern static bool Entity_Remove(uint entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Entity_IsValidEntityID(uint entityID);

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
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool SpriteComponent_SetDisable(uint entityID, bool disable);

        /* SortingGroupComponent */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool SortingGroupComponent_GetSortingLayerName(uint entityID, out string outLayerName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool SortingGroupComponent_SetSortingLayerName(uint entityID, string layerName);

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
        // New API
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_Play(uint entityID, string animationName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_Stop(uint entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_Pause(uint entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_AddAnimation(uint entityID, string assetName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_GetLooping(uint entityID, out bool looping);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_SetLooping(uint entityID, bool looping);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_IsPlaying(uint entityID, string animationName, out bool isPlaying);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_GetWrapMode(uint entityID, out int outWrapMode);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool AnimatedSpriteComponent_SetWrapMode(uint entityID, int wrapMode);

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
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool RigidBody2DComponent_InitializePhysics2DBody(uint entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool RigidBody2DComponent_SetDisabled(uint entityID, bool enabled);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool RigidBody2DComponent_IsDisabled(uint entityID, out bool enabled);


        /* BoxCollider2DComponent */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_GetShape(uint entityID, out int outShape);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_SetShape(uint entityID, int shape);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_GetOffset(uint entityID, out Vector2 outOffset);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_SetOffset(uint entityID, ref Vector2 shape);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_GetSize(uint entityID, out Vector2 outSize);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_SetSize(uint entityID, ref Vector2 size);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_GetDensity(uint entityID, out float outDensity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_SetDensiry(uint entityID, float density);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_GetFriction(uint entityID, out float outFriction);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_SetFriction(uint entityID, float friction);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_GetRestitution(uint entityID, out float outRestitution);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_SetRestitution(uint entityID, float restitution);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_GetRestitutionThreshold(uint entityID, out float outRestitutionThreshold);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool BoxCollider2DComponent_SetRestitutionThreshold(uint entityID, float restitutionThreshold);

        /* CameraComponent */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool CameraComponent_GetIsMain(uint entityID, out bool isMain);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool CameraComponent_SetIsMain(uint entityID, bool isMain);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool CameraComponent_GetZoom(uint entityID, out float zoom);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool CameraComponent_SetZoom(uint entityID, ref float zoom);

        /* Input - Keyboard */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Input_IsKeyDown(KeyCode keycode);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Input_IsKeyPressed(KeyCode keycode);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Input_IsKeyUp(KeyCode keycode);
        /* Input - Mouse */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Input_IsMouseDown(MouseCode mousecode);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Input_IsMousePressed(MouseCode mousecode);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Input_IsMouseUp(MouseCode mousecode);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Input_GetMousePosition(out Vector2 position);

        /* Physics */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Physics_ApplyForce(uint entityID, ref Vector2 force, ref Vector2 point, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Physics_ApplyForceCenter(uint entityID, ref Vector2 force, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Physics_ApplyLinearImpulse(uint entityID, ref Vector2 impulse, ref Vector2 point, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Physics_ApplyLinearImpulseCenter(uint entityID, ref Vector2 impulse, bool wake);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Physics_GetLinearVelocity(uint entityID, out Vector2 velocity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Physics_SetLinearVelocity(uint entityID, ref Vector2 velocity);

        /* Collision */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Collision_GetImpactVelocity(uint OwnerID, uint TargetID, out Vector2 velocity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static Contact[] Collision_GetContacts(uint OwnerID, uint TargetID);

        /* Scripting */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static object GetScriptInstace(uint entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static object CreateScriptInstance(string name);
    }
}
