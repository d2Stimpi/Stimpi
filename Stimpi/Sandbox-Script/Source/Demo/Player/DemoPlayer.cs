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
    [ScriptOrder(Priority = 1)]
    public class DemoPlayer : Entity
    {
        private QuadComponent Quad;
        public float Velocity = 20.0f;

        public Entity Camera;
        public Entity Cursor;
        private QuadComponent CursorQuad;

        public override void OnCreate()
        {
            // FIXME: This is temporary solution, add OnDestroy to scripts
            // Clear all instances in ObjectPool(s) on "Scene Start"
            EffectsPool.Clear();

            Quad = GetComponent<QuadComponent>();
            if (Cursor != null)
            {
                CursorQuad = Cursor.GetComponent<QuadComponent>();
            }
        }

        public override void OnUpdate(float ts)
        {
            // Tracking "dummy" object - just to show where the target actually is (cursor pos)
            if (CursorQuad != null)
            {
                Vector2 mousePos = Input.GetMousePosition();
                CursorQuad.Position = mousePos;
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

            // Spawn a bullet
            if (Input.IsKeyDown(KeyCode.KEY_SPACE))
            {
                Bullet bullet = Entity.Create<Bullet>();

                float size = bullet.GetSize().X;
                Vector2 mousePos = Input.GetMousePosition();
                Vector2 fireLocation = Quad.Position;

                Vector2 dir = mousePos - fireLocation;
                fireLocation += dir.Unit * size;

                bullet.Initialize(this, CursorQuad.Position, fireLocation);
            }
        }
    }
}
