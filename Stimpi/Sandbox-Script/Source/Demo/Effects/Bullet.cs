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
        private Entity _owner;
        private QuadComponent _quad;

        public float Velocity =  100.0f;
        public float LifeSpan = 2.0f; // in seconds

        public void Initialize(Entity owner, Vector2 target, Vector2 start)
        {
            _owner = owner;
            _quad.Position = start;

            Vector2 ownerPos;
            QuadComponent ownerQuad = _owner.GetComponent<QuadComponent>();
            if (ownerQuad != null)
                ownerPos = ownerQuad.Position;
            else
                ownerPos = start;

            Vector2 angleDir = target - ownerPos;
            // Calculate rotation angle between X-Axis and Target vector
            float angle = Vector2.AxisX.Angle(angleDir.Unit);
            Console.WriteLine($"Bullet's angle: {angle}");
           
            if (angleDir.Y >= 0)
                _quad.Rotation = angle;
            else
                _quad.Rotation = -angle;

            Physics.InitializePhysics2DBody(ID);

            Vector2 dir = target - start;
            Vector2 vecVec = dir.Unit * Velocity;
            Physics.SetLinearVelocity(ID, vecVec);
        }

        public Vector2 GetSize()
        {
            return _quad.Size;
        }

        public override void OnCreate()
        {
            _quad = AddComponent<QuadComponent>();
            _quad.Size = new Vector2(6.0f, 6.0f);

            //SpriteComponent sprite = AddComponent<SpriteComponent>();
            //sprite.Color = Color.Black;

            AnimatedSpriteComponent anim = AddComponent<AnimatedSpriteComponent>();
            anim.AddAnimation("animations\\fireball01.anim");
            anim.Play("fireball01.anim");
            anim.Looping = true;

            BoxCollider2DComponent bc2d = AddComponent<BoxCollider2DComponent>();
            bc2d.Shape = Collider2DShape.CIRCLE;
            bc2d.Size = new Vector2(0.25f, 0.25f);  // Weird, but it means 2 times smaller collider than sprite

            RigidBody2DComponent rb2d = AddComponent<RigidBody2DComponent>();
            rb2d.Type = BodyType.DYNAMIC;
            rb2d.FixedRotation = true;
        }

        public override void OnUpdate(float ts)
        {
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
