using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class TagComponent : Component
    {
        public string Tag
        { 
            get
            {
                InternalCalls.TagComponent_GetString(Entity.ID, out string retTag);
                return retTag;
            }
        }
    }

    public class QuadComponent : Component
    {
        public Vector2 Position
        {
            get
            {
                if (!InternalCalls.QuadComponent_GetPosition(Entity.ID, out Vector2 retPos))
                    Console.WriteLine($"Entity {Entity.ID} does not have QuadComonent (get pos)");

                return retPos;
            }
            set
            {
                if (!InternalCalls.QuadComponent_SetPosition(Entity.ID, ref value))
                    Console.WriteLine($"Entity {Entity.ID} does not have QuadComonent (set pos)");
            }
        }
        public Vector2 Size
        {
            get
            {
                if (!InternalCalls.QuadComponent_GetSize(Entity.ID, out Vector2 retSize))
                    Console.WriteLine($"Entity {Entity.ID} does not have QuadComonent (get size)");

                return retSize;
            }
            set
            {
                if (!InternalCalls.QuadComponent_SetSize(Entity.ID, ref value))
                    Console.WriteLine($"Entity {Entity.ID} does not have QuadComonent (set size)");
            }
        }

        public float Rotation
        {
            get
            {
                if (!InternalCalls.QuadComponent_GetRotation(Entity.ID, out float outRotation))
                    Console.WriteLine($"Entity {Entity.ID} does not have QuadComonent (get rotation)");

                return outRotation;
            }
            set
            {
                if (!InternalCalls.QuadComponent_SetRotation(Entity.ID, ref value))
                    Console.WriteLine($"Entity {Entity.ID} does not have QuadComonent (set rotation)");
            }
        }
    }

    public class SpriteComponent : Component
    {
        public Color Color
        {
            get
            {
                if (!InternalCalls.SpriteComponent_GetColor(Entity.ID, out Color outColor))
                    Console.WriteLine($"Entity {Entity.ID} does not have SpriteComponent (get color)");

                return outColor;
            }
            set
            {
                if (!InternalCalls.SpriteComponent_SetColor(Entity.ID, ref value))
                    Console.WriteLine($"Entity {Entity.ID} does not have SpriteComponent (set color)");
            }
        }
    }

    public enum AnimationState
    {
        RUNNING = 0, 
        PAUSED,
        STOPPED,
        COMPELETED
    }

    public class AnimatedSpriteComponent : Component
    {
        public bool IsAnimationSet()
        {
            if (!InternalCalls.AnimatedSpriteComponent_IsAnimationSet(Entity.ID, out bool isSet))
                Console.WriteLine($"Entity {Entity.ID} does not have AnimatedSpriteComponent (set Start state)");

            return isSet;
        }

        public void AnimStart()
        {
            if (!InternalCalls.AnimatedSpriteComponent_AnimStart(Entity.ID))
                Console.WriteLine($"Entity {Entity.ID} does not have AnimatedSpriteComponent (set Start state)");
        }

        public void AnimPause()
        {
            if (!InternalCalls.AnimatedSpriteComponent_AnimPause(Entity.ID))
                Console.WriteLine($"Entity {Entity.ID} does not have AnimatedSpriteComponent (set Pause state)");
        }

        public void AnimStop()
        {
            if (!InternalCalls.AnimatedSpriteComponent_AnimStop(Entity.ID))
                Console.WriteLine($"Entity {Entity.ID} does not have AnimatedSpriteComponent (set Stop state)");
        }

        public AnimationState AnimState
        {
            get
            {
                if (!InternalCalls.AnimatedSpriteComponent_GetAnimState(Entity.ID, out int outState))
                    Console.WriteLine($"Entity {Entity.ID} does not have AnimatedSpriteComponent (get Anim state)");

                return (AnimationState)outState;
            }
        }
    }

    public enum BodyType
    {
        STATIC = 0,
        DYNAMIC,
        KINETIC
    }

    public class RigidBody2DComponent : Component
    {
        public BodyType Type
        {
            get
            {
                if (!InternalCalls.RigidBody2DComponent_GetRigidBodyType(Entity.ID, out int outType))
                    Console.WriteLine($"Entity {Entity.ID} does not have RigidBody2DComponent (get type)");

                return (BodyType)outType;
            }
            set
            {
                if (!InternalCalls.RigidBody2DComponent_SetRigidBodyType(Entity.ID, (int)value))
                    Console.WriteLine($"Entity {Entity.ID} does not have RigidBody2DComponent (set type)");
            }
        }

        public bool FixedRotation
        {
            get
            {
                if (!InternalCalls.RigidBody2DComponent_GetFixedRotation(Entity.ID, out bool outFixedRotation))
                    Console.WriteLine($"Entity {Entity.ID} does not have RigidBody2DComponent (get type)");

                return outFixedRotation;
            }
            set
            {
                if (!InternalCalls.RigidBody2DComponent_SetFixedRotation(Entity.ID, value))
                    Console.WriteLine($"Entity {Entity.ID} does not have RigidBody2DComponent (set type)");
            }
        }

        public bool GetTransform(out Vector2 position, out float angle)
        {
            bool retVal = InternalCalls.RigidBody2DComponent_GetTransform(Entity.ID, out Vector2 pos, out float ang);
            position = pos;
            angle = ang;
            return retVal;
        }

        public bool SetTransform(Vector2 position, float angle)
        {
            return InternalCalls.RigidBody2DComponent_SetTransform(Entity.ID, ref position, ref angle);
        }
    }

    public class BoxCollider2DComponent : Component
    {

    }

    public class CameraComponent : Component
    {
        public bool IsMain
        {
            get
            {
                if (!InternalCalls.CameraComponent_GetIsMain(Entity.ID, out bool outIsMain))
                    Console.WriteLine($"Entity {Entity.ID} does not have CameraComponent (get isMain)");

                return outIsMain;
            }
            set
            {
                if (!InternalCalls.CameraComponent_SetIsMain(Entity.ID, value))
                    Console.WriteLine($"Entity {Entity.ID} does not have CameraComponent (set isMain)");
            }
        }

        public Vector2 Position
        {
            get
            {
                if (!InternalCalls.QuadComponent_GetPosition(Entity.ID, out Vector2 retPos))
                    Console.WriteLine($"Entity {Entity.ID} does not have Camera-QuadComonent (get pos)");

                return retPos;
            }
            set
            {
                if (!InternalCalls.QuadComponent_SetPosition(Entity.ID, ref value))
                    Console.WriteLine($"Entity {Entity.ID} does not have Camera-QuadComonent (set pos)");
            }
        }

        public float Zoom
        {
            get
            {
                if (!InternalCalls.CameraComponent_GetZoom(Entity.ID, out float zoom))
                    Console.WriteLine($"Entity {Entity.ID} does not have CameraComonent (get zoom)");

                return zoom;
            }
            set
            {
                if (!InternalCalls.CameraComponent_SetZoom(Entity.ID, ref value))
                    Console.WriteLine($"Entity {Entity.ID} does not have CameraComonent (set zoom)");
            }
        }
    }

    public class ScriptComponent : Component
    {

    }
}
