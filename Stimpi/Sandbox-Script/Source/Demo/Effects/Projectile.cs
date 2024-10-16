using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{
    public class Projectile : Entity
    {
        private Entity _owner;
        private QuadComponent _quad;
        private RigidBody2DComponent _rb2d;
        private AnimatedSpriteComponent _anim;
        private bool _enableUpdate = true;
        private float _fileSpan = 0.0f;
        private Vector2 _velocity;

        public float Velocity = 100.0f;
        public float LifeSpan = 1.0f; // in seconds

        public void Initialize(Entity owner, Vector2 target, Vector2 size)
        {
            _enableUpdate = true;
            _fileSpan = LifeSpan;
            _owner = owner;

            // Get owner Position
            Vector2 ownerPos = Vector2.Zero;
            QuadComponent ownerQuad = _owner.GetComponent<QuadComponent>();
            if (ownerQuad != null)
                ownerPos = ownerQuad.Position;

            // Calculate where to spawn Projectile
            Vector2 firePos = ownerPos + (target - ownerPos).Unit * (size.X / 3 + size.Y / 3);
            // Calculate angle
            Vector2 dir = target - ownerPos;
            Vector2 vecVec = dir.Unit * Velocity;
            Vector2 angleDir = target - ownerPos;
            // Calculate rotation angle between X-Axis and Target vector
            float angle = Vector2.AxisX.Angle(angleDir.Unit);
            //Console.WriteLine($"Bullet's angle: {angle}");

            _quad.Position = firePos;
            _quad.Size = size;
            if (angleDir.Y >= 0)
                _quad.Rotation = angle;
            else
                _quad.Rotation = -angle;

            Physics.InitializePhysics2DBody(ID);    // Will only create the body if it does not already exist

            _rb2d.Enabled = true;
            _rb2d.SetTransform(_quad.Position, _quad.Rotation);
            _velocity = vecVec;
            Physics.SetLinearVelocity(ID, _velocity);

            Physics.GetLinearVelocity(ID, out Vector2 outVel);
        }

        public void Invalidate(Vector2 pos)
        {
            // Physics.SetDisabled for pooled physics objects

            Physics.SetLinearVelocity(ID, Vector2.Zero);
            _rb2d.Enabled = false;
            _rb2d.SetTransform(pos, 0);
            _anim.Looping = false;
            _enableUpdate = false;
        }

        public void SetAndPlayAnimation(string name, bool loop)
        {
            _anim.Play(name);
            _anim.Looping = loop;
        }

        public override void OnCreate()
        {
            _quad = AddComponent<QuadComponent>();
            _quad.Size = new Vector2(9.0f, 9.0f);

            _anim = AddComponent<AnimatedSpriteComponent>();
            _anim.AddAnimation("animations\\fireball01.anim");
            _anim.AddAnimation("animations\\lightning_proj_start.anim");
            _anim.AddAnimation("animations\\lightning_proj_loop.anim");
            //_anim.Play("fireball01.anim");
            _anim.Looping = false;

            BoxCollider2DComponent bc2d = AddComponent<BoxCollider2DComponent>();
            bc2d.Shape = Collider2DShape.CIRCLE;
            bc2d.Size = new Vector2(0.25f, 0.25f);  // Weird, but it means 2 times smaller collider than sprite

            _rb2d = AddComponent<RigidBody2DComponent>();
            _rb2d.Type = BodyType.DYNAMIC;
            _rb2d.FixedRotation = true;
        }

        public override void OnUpdate(float ts)
        {
            if (_enableUpdate)
            {
                Physics.SetLinearVelocity(ID, _velocity);
                _fileSpan -= ts;
                if (_fileSpan <= 0)
                {
                    //Entity.Destroy(ID);
                    Console.WriteLine($"Projectile's lifespan elapsed");
                    ProjectileFactory.Release(this);
                }
            }
        }

        public override void OnCollisionBegin(Collision collision)
        {
            bool collide = true;
            var other = collision.OtherID;

            Entity otherEnt = FindEntityByID(other);
            string tag = otherEnt.GetComponent<TagComponent>().Tag;
            Console.WriteLine($"Collided with {tag}");
            if (/*tag == "Immovable_1" || */tag == "Player" || tag == "Demo.Projectile")
                collide = false;
            
            var obj = InternalCalls.GetScriptInstace(other);
            if (obj != null)
            {
                var type = obj.GetType();
                Console.WriteLine($"Collision wiht obj type {type.Name}");
            }
            
            if (collide)
            {
                Explosion explosion = EffectsPool.GetObject();
                Vector2 impact = collision.Contacts[0].Point;
                explosion.Initialize(impact);
                explosion.Play();

                //Entity.Destroy(ID);
                ProjectileFactory.Release(this);
            }
        }

        public override void OnCollisionEnd(Collision collision)
        {
        }

        public override bool OnCollisionPreSolve(Collision collision)
        {
            return false;
        }
    }

}
