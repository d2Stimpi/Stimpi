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
}
