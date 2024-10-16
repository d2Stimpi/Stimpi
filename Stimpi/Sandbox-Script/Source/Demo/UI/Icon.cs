using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{
    public class Icon : Entity
    {
        private Entity Camera;
        private QuadComponent Quad;
        private SpriteComponent Sprite;

        public Vector2 IconPosition;

        public void Disable()
        {
            if (Sprite != null)
            {
                Sprite.SetDisabled(true);
            }
        }

        public void Enable()
        {
            Sprite = GetComponent<SpriteComponent>();
            if (Sprite != null)
            {
                Sprite.SetDisabled(false);
            }
        }

        public void SetColor(Color color)
        {
            Sprite = GetComponent<SpriteComponent>();
            if (Sprite != null)
            {
                Sprite.Color = color;
            }
        }

        public override void OnCreate()
        {
            Camera = FindEntityByName("MainCamera");
            Quad = GetComponent<QuadComponent>();
            Sprite = GetComponent<SpriteComponent>();

            IconPosition = Quad.Position;
            if (Sprite != null)
                Sprite.SetDisabled(true);
        }

        public override void OnUpdate(float ts)
        {
            if (ID == 0)
            {
                Console.WriteLine("Icon got busted?!");
                return;
            }

            Camera = FindEntityByName("MainCamera");
            Quad = GetComponent<QuadComponent>();
            if (Camera != null && Quad != null)
            {
                QuadComponent camQuad = Camera.GetComponent<QuadComponent>();
                if (camQuad != null)
                    Quad.Position = camQuad.Position + IconPosition;
                else
                    Console.WriteLine("Camera got busted?!");
            }
        }

        public override string ToString()
        {
            return GetComponent<TagComponent>().Tag;
        }
    }
}
