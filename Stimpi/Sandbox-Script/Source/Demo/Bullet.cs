using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{
    class Bullet : Entity
    {
        private Entity Owner;
        private QuadComponent Quad;

        public float Velocity =  100.0f;
        public float LifeSpan = 2.0f; // in seconds

        public void Initialize(Entity owner, Vector2 target, Vector2 start)
        {
            Owner = owner;
            Quad.Position = start;

            Physics.InitializePhysics2DBody(ID);

            Vector2 dir = target - start;
            Vector2 vecVec = dir.Unit * Velocity;
            Physics.SetLinearVelocity(ID, vecVec);
        }

        public override void OnCreate()
        {
            Quad = AddComponent<QuadComponent>();
            Quad.Size = new Vector2(3.0f, 3.0f);

            SpriteComponent sprite = AddComponent<SpriteComponent>();
            sprite.Color = Color.Black;

            AddComponent<BoxCollider2DComponent>();
            RigidBody2DComponent rb2d = AddComponent<RigidBody2DComponent>();
            rb2d.Type = BodyType.DYNAMIC;
            rb2d.FixedRotation = true;
        }

        public override void OnUpdate(float ts)
        {
            //Console.WriteLine($"Elapsed: {LifeSpan}");
            LifeSpan -= ts;
            if (LifeSpan <= 0)
                Entity.Destroy(ID);
        }

        public override void OnCollisionBegin(Collision collision)
        {
            //Explosion explosion = Entity.Create<Explosion>();
            Explosion explosion = EffectsPool.GetObject();
            Vector2 impact = collision.Contacts[0].Point;
            explosion.Initialize(impact);
            explosion.Play();

            Entity.Destroy(ID);
        }

        public override void OnCollisionEnd(Collision collision)
        {
        }
    }

}
