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
            get;
            set;
        }

        public float Rotation
        {
            get;
            set;
        }
    }

}
