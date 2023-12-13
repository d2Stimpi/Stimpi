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

        public override void OnCreate()
        {

        }

        public override void OnUpdate(float ts)
        {
            if (Input.IsKeyPressed(KeyCode.KEY_SPACE))
            {
                InternalCalls.Physics_ApplyForceCenter(ID, ref force, false);
            }
        }
    }
}
