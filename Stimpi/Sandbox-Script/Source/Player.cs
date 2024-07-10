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

        public Entity camera;
        public Vector2 vector;

        public override void OnCreate()
        {
            Console.WriteLine($"OnCreate: {ID}");

            m_Quad = GetComponent<QuadComponent>();

            if (HasComponent<AnimatedSpriteComponent>())
                m_AnimComponent = GetComponent<AnimatedSpriteComponent>();

            /*camera = FindEntityByName("Camera");
            Console.WriteLine($"Camera found - ID {camera.ID}");*/
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

                m_Quad.Position = pos;
                m_Quad.Size = size;
                m_Quad.Rotation = rotation;

                // Update camera to follow player
                //Entity camera = FindEntityByName("Camera");
                if (camera != null)
                {
                    //Console.WriteLine("Camera found");
                    QuadComponent quad = camera.GetComponent<QuadComponent>();
                    Vector2 offset = new Vector2(-50f, -30f);
                    quad.Position = pos + offset;

                    if (Input.IsKeyPressed(KeyCode.KEY_KP_PLUS))
                    {
                        Vector2 camSize = quad.Size;
                        quad.Size = camSize + new Vector2(10.0f, 10.0f);
                    }
                    if (Input.IsKeyPressed(KeyCode.KEY_KP_MINUS))
                    {
                        Vector2 camSize = quad.Size;
                        quad.Size = camSize - new Vector2(10.0f, 10.0f);
                    }
                }

            }
        }
    }
}
