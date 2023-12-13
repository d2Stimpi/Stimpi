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
        private Vector2 m_SavePosition;
        private Vector2 m_SaveSize;
        private float m_SaveRotation;

        private SpriteComponent m_Sprite;
        public float colorRatio = 1.0f / 255.0f;
        public float speed = 15.0f;

        public override void OnCreate() 
        {
            string tag = GetComponent<TagComponent>().Tag;
            Console.WriteLine($"OnCreate Actor - {tag}");

            m_Quad = GetComponent<QuadComponent>();
            m_Sprite = GetComponent<SpriteComponent>();

            RigidBody2DComponent rb2d = GetComponent<RigidBody2DComponent>();
            if (rb2d != null)
            {
                Console.WriteLine($"Rigid Body type used: {rb2d.Type}");
                rb2d.Type = BodyType.DYNAMIC;
                Console.WriteLine($"Rigid Body fixed rotation: {rb2d.FixedRotation}");
                rb2d.FixedRotation = true;
            }
        }

        public override void OnUpdate(float ts)
        {            
            if (Input.IsKeyPressed(KeyCode.KEY_1) && Input.IsKeyPressed(KeyCode.KEY_LCTRL))
            {
                if (m_Quad == null)
                {
                    Console.WriteLine("AddComponent<QuadComponent>");
                    m_Quad = AddComponent<QuadComponent>();

                    m_Quad.Position = m_SavePosition;
                    m_Quad.Size = m_SaveSize;
                    m_Quad.Rotation = m_SaveRotation;
                }
            }

            if (Input.IsKeyPressed(KeyCode.KEY_2) && Input.IsKeyPressed(KeyCode.KEY_LCTRL))
            {
                if (m_Quad != null)
                {
                    m_SavePosition = m_Quad.Position;
                    m_SaveSize = m_Quad.Size;
                    m_SaveRotation = m_Quad.Rotation;

                    Console.WriteLine("RemoveComponent<QuadComponent>");
                    RemoveComponent<QuadComponent>();
                    m_Quad = null;
                }
            }

            // Check if QuadComponent is available
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
            }

            if (m_Sprite != null)
            {
                Color color = m_Sprite.Color;
                color.R += colorRatio * ts * speed;
                color.R %= 1.0f;
                m_Sprite.Color = color;
            }
        }
    }
}
