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

        public float jumpForce = 10.0f;

        public override void OnCreate()
        {

        }

        public override void OnUpdate(float ts)
        {
            if (Input.IsKeyPressed(KeyCode.KEY_SPACE))
            {
                Console.WriteLine("JUMP! JUMP!");
                Vector2 forceVector = new Vector2(0.0f, jumpForce);
                Physics.ApplyImpulseCenter(ID, forceVector, false);
            }
        }

        public override void OnCollisionBegin(Collision collision)
        {
            base.OnCollisionBegin(collision);
            Vector2 f = new Vector2(0.0f, 300.5f);
            Physics.ApplyImpulseCenter(ID, f, false);
        }

        public override void OnCollisionEnd(Collision collision)
        {
            base.OnCollisionEnd(collision);
        }
    }
}
