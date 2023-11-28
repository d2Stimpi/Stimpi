using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Sandbox
{
    class Actor : Entity
    {
        private QuadComponent m_Quad;

        public override void OnCreate() 
        {
            Console.WriteLine("OnCreate Actor");

            m_Quad = GetComponent<QuadComponent>();
        }

        public override void OnUpdate(float ts)
        {
            float speed = 15.0f;

            Vector2 pos = m_Quad.Position;

            if (Input.IsKeyPressed(KeyCode.KEY_W))
                pos.Y += ts * speed;
            if (Input.IsKeyPressed(KeyCode.KEY_S))
                pos.Y -= ts * speed;
            if (Input.IsKeyPressed(KeyCode.KEY_D))
                pos.X += ts * speed;
            if (Input.IsKeyPressed(KeyCode.KEY_A))
                pos.X -= ts * speed;

            m_Quad.Position = pos;
        }
    }
}
