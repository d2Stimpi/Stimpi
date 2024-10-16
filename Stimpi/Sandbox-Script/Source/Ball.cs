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
                Physics.SetLinearVelocity(ID, impulse);
            }

            if (Input.IsKeyPressed(KeyCode.KEY_A))
            {
                Vector2 impulse = new Vector2(-move, 0.0f);
                Physics.SetLinearVelocity(ID, impulse);
            }

            if (Input.IsKeyPressed(KeyCode.KEY_SPACE))
            {
                Vector2 impulse = new Vector2(0.0f, jump);
                Physics.SetLinearVelocity(ID, impulse);
            }

            var Camera = FindEntityByName("MainCamera");
            var Quad = GetComponent<QuadComponent>();
            if (Camera != null && Quad != null)
            {
                QuadComponent camQuad = Camera.GetComponent<QuadComponent>();
                if (camQuad != null)
                    camQuad.Position = Quad.Position + new Vector2(-64, -36);
                else
                    Console.WriteLine("Camera got busted?!");
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
