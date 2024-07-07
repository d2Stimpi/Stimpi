using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Sandbox
{
    class PhysicsActor : Entity
    {
        private Vector2 force = new Vector2(0.0f, 2.5f);

        public float jumpForce = 1000.0f;

        public override void OnCreate()
        {
            Entity camera = FindEntityByName("Camera");
            if (camera != null)
            {
                QuadComponent quad = camera.GetComponent<QuadComponent>();
                Vector2 camPos = quad.Position;
                camPos.X += 50;
                //quad.Position = camPos;
            }

            Entity playerEntity = FindEntityByName("Player");
            if (playerEntity != null)
            {
                Actor player = playerEntity.As<Actor>();
                //Console.WriteLine("Player found");
                player.speed = 30.0f;
            }
        }

        public override void OnUpdate(float ts)
        {
            if (Input.IsKeyPressed(KeyCode.KEY_SPACE))
            {
                Console.WriteLine("JUMP! JUMP!");
                Vector2 forceVector = new Vector2(0.0f, jumpForce);
                Physics.ApplyImpulseCenter(ID, forceVector, false);
            }

            if (Input.IsKeyPressed(KeyCode.KEY_Y))
            {
                RigidBody2DComponent comp = GetComponent<RigidBody2DComponent>();
                if (comp != null)
                {
                    comp.GetTransform(out Vector2 transformPos, out float angle);
                    //Console.WriteLine($"Body Transform: pos = {transformPos.X}, {transformPos.Y} angle = {angle}");

                    Vector2 newPos = new Vector2(transformPos.X + 1, transformPos.Y);
                    comp.SetTransform(newPos, angle);
                }
            }

            if (Input.IsKeyPressed(KeyCode.KEY_R))
            {
                RigidBody2DComponent comp = GetComponent<RigidBody2DComponent>();
                if (comp != null)
                {
                    comp.GetTransform(out Vector2 transformPos, out float angle);
                    Vector2 newPos = new Vector2(transformPos.X - 1, transformPos.Y);
                    comp.SetTransform(newPos, angle);
                }
            }

            if (Input.IsKeyPressed(KeyCode.KEY_T))
            {
                RigidBody2DComponent comp = GetComponent<RigidBody2DComponent>();
                if (comp != null)
                {
                    comp.GetTransform(out Vector2 transformPos, out float angle);
                    Vector2 newPos = new Vector2(transformPos.X, transformPos.Y + 1);
                    comp.SetTransform(newPos, angle);
                }
            }
        }

        public override void OnCollisionBegin(Collision collision)
        {
            base.OnCollisionBegin(collision);

            bool log = false;

            Contact[] contacts = collision.Contacts;
            if (contacts != null)
            {
                if (log) Console.WriteLine($"Contact impact velocity: {collision.ImpactVelocty.X}, {collision.ImpactVelocty.Y}");
                if (log) Console.WriteLine($"Contact points count: {contacts.Length}");
                foreach (Contact contact in contacts)
                {
                    if (contact != null)
                    {
                        if (log) Console.WriteLine($"Contact point: {contact.Point.X}, {contact.Point.Y}");
                        if (log) Console.WriteLine($"Contact point impact velocity: {contact.ImpactVelocty.X}, {contact.ImpactVelocty.Y}");
                    }
                    else
                        if (log) Console.WriteLine("Invalid contact !?");
                }
            }
            else
            {
                if (log) Console.WriteLine("Contacts is null");
            }

            Vector2 f = new Vector2(0.0f, jumpForce);
            Physics.ApplyImpulseCenter(ID, f, false);
        }

        public override void OnCollisionEnd(Collision collision)
        {
            base.OnCollisionEnd(collision);
        }
    }
}
