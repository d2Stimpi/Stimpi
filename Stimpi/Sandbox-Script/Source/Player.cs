using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Sandbox
{
    class Player : Entity
    {
        private AnimatedSpriteComponent m_AnimComponent;
        private QuadComponent m_Quad;
        public float speed = 16.0f;

        public override void OnCreate()
        {
            Console.WriteLine($"OnCreate: {ID}");

            m_Quad = GetComponent<QuadComponent>();

            if (HasComponent<AnimatedSpriteComponent>())
                m_AnimComponent = GetComponent<AnimatedSpriteComponent>();
        }

        public override void OnUpdate(float ts)
        {
            if(Input.IsKeyPressed(KeyCode.KEY_SPACE))
            {
                if (m_AnimComponent != null)
                {
                    AnimationState animState = m_AnimComponent.AnimState;
                    if (animState != AnimationState.RUNNING)
                    {
                        m_AnimComponent.AnimStart();
                    }
                }
            }

            if (m_Quad != null)
            {
                Vector2 pos = m_Quad.Position;

                if (Input.IsKeyPressed(KeyCode.KEY_W))
                    pos.Y += ts * speed;
                if (Input.IsKeyPressed(KeyCode.KEY_S))
                    pos.Y -= ts * speed;
                if (Input.IsKeyPressed(KeyCode.KEY_D))
                    pos.X += ts * speed;
                if (Input.IsKeyPressed(KeyCode.KEY_A))
                    pos.X -= ts * speed;

                Vector2 size = m_Quad.Size;
                if (Input.IsKeyPressed(KeyCode.KEY_X))
                {
                    size.X += ts * speed;
                    size.Y += ts * speed;
                }
                if (Input.IsKeyPressed(KeyCode.KEY_C))
                {
                    size.X -= ts * speed;
                    size.Y -= ts * speed;
                }

                float rotation = m_Quad.Rotation;
                if (Input.IsKeyPressed(KeyCode.KEY_Q))
                    rotation += ts * speed;
                if (Input.IsKeyPressed(KeyCode.KEY_E))
                    rotation -= ts * speed;

                // Round the position to 1 decimal
                /*string str = pos.X.ToString("n1");
                pos.X = Convert.ToSingle(str);
                str = pos.Y.ToString("n1");
                pos.Y = Convert.ToSingle(str);
                Console.WriteLine($"Converted Pos to: {pos}");*/

                m_Quad.Position = pos;
                m_Quad.Size = size;
                m_Quad.Rotation = rotation;

                // Update camera to follow player
                Entity camera = FindEntityByName("Camera");
                if (camera != null)
                {
                    //Console.WriteLine("Camera found");
                    QuadComponent quad = camera.GetComponent<QuadComponent>();
                    Vector2 offset = new Vector2(-50f, -30f);
                    quad.Position = pos + offset;
                }

            }
        }
    }
}
