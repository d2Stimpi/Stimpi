using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

// TODO - script internal calls
// Get Window size or Camera view box

namespace Demo
{
    public class DemoPlayer : Entity
    {
        private QuadComponent Quad;
        public float Velocity = 20.0f;

        public Entity Camera;
        public Entity Cursor;
        private QuadComponent CursorQuad;

        public override void OnCreate()
        {
            Quad = GetComponent<QuadComponent>();
            if (Cursor != null)
            {
                CursorQuad = Cursor.GetComponent<QuadComponent>();
                Console.WriteLine("Cursor quad ref created");
            }
        }

        public override void OnUpdate(float ts)
        {
            // Tracking "dummy" object - just to show where the target actually is (cursor pos)
            if (CursorQuad != null)
            {
                Vector2 mousePos = Input.GetMousePosition();
                CursorQuad.Position = mousePos;
                //Console.WriteLine($"Cursor quad new pos {CursorQuad.Position} , mouse pos {mousePos}");
            }

            if (Quad != null)
            {
                HandleMoveInput();
            }

            // Updated attached camera to player object
            if (Camera != null)
            {
                var camQuad = Camera.GetComponent<QuadComponent>();
                camQuad.Position = Quad.Position + new Vector2(-64, -36);
            }
        }

        public override void OnCollisionBegin(Collision collision)
        {
        }

        public override void OnCollisionEnd(Collision collision)
        {
        }

        private void HandleMoveInput()
        {
            if (Input.IsMousePressed(MouseCode.MOUSE_BUTTON_LEFT) || Input.IsMousePressed(MouseCode.MOUSE_BUTTON_RIGHT))
            {
                Vector2 mousePos = Input.GetMousePosition();
                Vector2 pos = Quad.Position;

                Vector2 dir = mousePos - pos;
                Vector2 vecVec = dir.Unit * Velocity;
                if (Input.IsMousePressed(MouseCode.MOUSE_BUTTON_LEFT))
                    Physics.SetLinearVelocity(ID, vecVec);
                else
                    Physics.SetLinearVelocity(ID, vecVec.Inv);
            }
            else
            {
                Physics.SetLinearVelocity(ID, Vector2.Zero);
            }

            // Test - create a bullet
            if (Input.IsKeyDown(KeyCode.KEY_SPACE))
            {
                Bullet bullet = Entity.Create<Bullet>();
                Console.WriteLine("Setting target for Bullet");
                bullet.TargetEntity = Cursor;
                bullet.StartPos = Quad.Position;
                bullet.InitializeTarget();
            }


            if (Input.IsKeyPressed(KeyCode.KEY_D))
            {
                Physics.GetLinearVelocity(ID, out Vector2 outVel);
                Vector2 vel = new Vector2(Velocity, outVel.Y);
                Physics.SetLinearVelocity(ID, vel);
            }

            if (Input.IsKeyUp(KeyCode.KEY_D))
            {
                Physics.GetLinearVelocity(ID, out Vector2 outVel);
                Vector2 vel = new Vector2(0.0f, outVel.Y);
                Physics.SetLinearVelocity(ID, vel);
            }

            if (Input.IsKeyPressed(KeyCode.KEY_A))
            {
                Physics.GetLinearVelocity(ID, out Vector2 outVel);
                Vector2 vel = new Vector2(-Velocity, outVel.Y);
                Physics.SetLinearVelocity(ID, vel);
            }

            if (Input.IsKeyUp(KeyCode.KEY_A))
            {
                Physics.GetLinearVelocity(ID, out Vector2 outVel);
                Vector2 vel = new Vector2(0.0f, outVel.Y);
                Physics.SetLinearVelocity(ID, vel);
            }

            if (Input.IsKeyPressed(KeyCode.KEY_W))
            {
                Physics.GetLinearVelocity(ID, out Vector2 outVel);
                Vector2 vel = new Vector2(outVel.X, Velocity);
                Physics.SetLinearVelocity(ID, vel);
            }

            if (Input.IsKeyUp(KeyCode.KEY_W))
            {
                Physics.GetLinearVelocity(ID, out Vector2 outVel);
                Vector2 vel = new Vector2(outVel.X, 0.0f);
                Physics.SetLinearVelocity(ID, vel);
            }

            if (Input.IsKeyPressed(KeyCode.KEY_S))
            {
                Physics.GetLinearVelocity(ID, out Vector2 outVel);
                Vector2 vel = new Vector2(outVel.X, -Velocity);
                Physics.SetLinearVelocity(ID, vel);
            }

            if (Input.IsKeyUp(KeyCode.KEY_S))
            {
                Physics.GetLinearVelocity(ID, out Vector2 outVel);
                Vector2 vel = new Vector2(outVel.X, 0.0f);
                Physics.SetLinearVelocity(ID, vel);
            }
        }
    }
}
