using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Stimpi
{
    class Ball : Entity
    {
        public float move = 10.0f;
        public float jump = 150.0f;

        public override void OnCreate()
        {
            
        }

        public override void OnUpdate(float ts)
        {
            if (Input.IsKeyPressed(KeyCode.KEY_D))
            {
                Vector2 impulse = new Vector2(move, 0.0f);
                Physics.ApplyImpulseCenter(ID, impulse, false);
            }

            if (Input.IsKeyPressed(KeyCode.KEY_A))
            {
                Vector2 impulse = new Vector2(-move, 0.0f);
                Physics.ApplyImpulseCenter(ID, impulse, false);
            }

            if (Input.IsKeyPressed(KeyCode.KEY_SPACE))
            {
                Vector2 impulse = new Vector2(0.0f, jump);
                Physics.ApplyImpulseCenter(ID, impulse, false);
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
