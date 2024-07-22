using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{
    public class Enemy : Entity
    {
        private EnemySpawn SpawnEntity;

        private QuadComponent Quad;
        private float Health = 100.0f;

        public float Velocity = 20.0f;

        public float ChangeDirFrequency = 2.0f;     // Spawn unit every SpawnFrequency seconds
        private float TimeTracker = 0;

        public void Initialize(EnemySpawn spawn, Vector2 position)
        {
            SpawnEntity = spawn;
            Quad.Position = position;

            Physics.InitializePhysics2DBody(ID);

            // Set random direction (Test)
            Random rnd = new Random();
            Vector2 dir = new Vector2(rnd.Next(0, 10), rnd.Next(0, 10));
            Vector2 vecVec = dir.Unit * Velocity;
            Physics.SetLinearVelocity(ID, vecVec);
        }        

        public override void OnCreate()
        {
            Quad = AddComponent<QuadComponent>();
            Quad.Size = new Vector2(5.0f, 5.0f);

            SpriteComponent sprite = AddComponent<SpriteComponent>();
            sprite.Color = new Color(0.25f, 0.9f, 0.25f, 1.0f);

            AddComponent<BoxCollider2DComponent>();
            RigidBody2DComponent rb2d = AddComponent<RigidBody2DComponent>();
            rb2d.Type = BodyType.DYNAMIC;
            rb2d.FixedRotation = false;
        }

        public override void OnUpdate(float ts)
        {
            UpdateMoveDirection(ts);
        }

        public override void OnCollisionBegin(Collision collision)
        {
            Entity other = FindEntityByID(collision.OtherID);
            if (other != null)
            {
                Bullet bullet = other.As<Bullet>();
                if (bullet != null) // If this is actually a Bullet type
                {
                    Health -= 50;

                    if (Health <= 0)
                    {
                        SpawnEntity.EnemyDestroyed();
                        Entity.Destroy(ID);
                    }
                }
            }
        }

        public override void OnCollisionEnd(Collision collision)
        {
        }

        public void UpdateMoveDirection(float ts)
        {
            TimeTracker += ts;
            if (TimeTracker >= ChangeDirFrequency)
            {
                TimeTracker = 0;

                // Follow player on each dir change
                Entity playerEnt = FindEntityByName("Player");
                if (playerEnt != null)
                {
                    //DemoPlayer player = playerEnt.As<DemoPlayer>();
                    Vector2 playerPos = playerEnt.GetComponent<QuadComponent>().Position;
                    Vector2 dir = playerPos - Quad.Position;

                    Vector2 vecVec = dir.Unit * Velocity;
                    Physics.SetLinearVelocity(ID, vecVec);
                }
                else
                {
                    Console.WriteLine("DemoPlayer not found");
                }
            }
        }
    }
}
