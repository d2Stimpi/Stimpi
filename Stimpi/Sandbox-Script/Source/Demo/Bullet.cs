using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timer;

using Stimpi;

namespace Demo
{
    class Bullet : Entity
    {
        private Entity Owner;
        private QuadComponent Quad;

        public Vector2 StartPos;
        public Entity TargetEntity; // For target direction of bullet
        public float Velocity =  55.0f;
        public float LifeSpan = 3.0f; // in seconds

        /**
         *  Calculate velocity vector
         */
        public void InitializeTarget()
        {
            if (TargetEntity == null)
            {
                Console.WriteLine("Bullet target not set!");
            }
            else
            {
                var targetPos = TargetEntity.GetComponent<QuadComponent>().Position;
                Vector2 pos = Quad.Position;
                Vector2 dir = targetPos - pos;
                Vector2 vecVec = dir.Unit * Velocity;
                Physics.SetLinearVelocity(ID, vecVec);
                Console.WriteLine("Bullet SetLinearVelocity " + vecVec);
            }
        }

        public override void OnCreate()
        {
            Console.WriteLine("Bullet OnCreate() " + ID);

            //TagComponent tag = GetComponent<TagComponent>();
            //tag.Tag = $"Bullet_{ID}";
            Quad = AddComponent<QuadComponent>();
            Quad.Position = StartPos;
            Quad.Size = new Vector2(3.0f, 3.0f);

            SpriteComponent sprite = AddComponent<SpriteComponent>();
            sprite.Color = Color.Black;

            RigidBody2DComponent rb2d = AddComponent<RigidBody2DComponent>();
            rb2d.Type = BodyType.DYNAMIC;
            rb2d.FixedRotation = true;
            Physics.InitializePhysics2DBody(ID);

            Console.WriteLine("Bullet OnCreate() end");
        }

        public override void OnUpdate(float ts)
        {
            
        }

        public override void OnCollisionBegin(Collision collision)
        {
        }

        public override void OnCollisionEnd(Collision collision)
        {
        }
    }

}
