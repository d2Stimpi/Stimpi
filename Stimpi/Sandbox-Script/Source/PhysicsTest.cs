using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Sandbox
{
    class PhysicsTest : Entity
    {
        public float Velocity = 10.0f;

        public override void OnCreate()
        {
        }

        public override void OnUpdate(float ts)
        {
            if (Input.IsKeyPressed(KeyCode.KEY_D))
            {
                Vector2 vel = new Vector2(Velocity, 0.0f);
                Physics.SetLinearVelocity(ID, vel);
            }

            if (Input.IsKeyUp(KeyCode.KEY_D))
            {
                Vector2 vel = new Vector2(0.0f, 0.0f);
                Physics.SetLinearVelocity(ID, vel);
            }

            if (Input.IsKeyPressed(KeyCode.KEY_A))
            {
                Vector2 vel = new Vector2(-Velocity, 0.0f);
                Physics.SetLinearVelocity(ID, vel);
            }

            if (Input.IsKeyUp(KeyCode.KEY_A))
            {
                Vector2 vel = new Vector2(0.0f, 0.0f);
                Physics.SetLinearVelocity(ID, vel);
            }
        }

        public override void OnCollisionBegin(Collision collision)
        {
        }

        public override void OnCollisionEnd(Collision collision)
        {
        }
    }
}
