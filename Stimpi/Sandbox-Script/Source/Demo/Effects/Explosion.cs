using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{
    public class Explosion : Entity
    {
        QuadComponent Quad;
        AnimatedSpriteComponent Animation;

        public Explosion()
        {
            //Console.WriteLine("Explosion ctor");
        }

        private bool Played = false;

        public void Initialize(Vector2 position)
        {
            Quad.Position = position;
            Random rnd = new Random();
            Quad.Rotation = rnd.Next(0, 3);
        }

        public void Play()
        {
            Animation.Play("explosion01.anim");
            Played = true;
        }

        public override void OnCreate()
        {
            Quad = AddComponent<QuadComponent>();
            Quad.Size = new Vector2(8.0f, 8.0f);

            Animation = AddComponent<AnimatedSpriteComponent>();
            Animation.AddAnimation("animations\\explosion01.anim");
        }

        public override void OnUpdate(float ts)
        {
            if (Played)
            {
                if (Animation.AnimState == AnimationState.COMPELETED)
                {
                    Played = false;                    
                    EffectsPool.ReleaseObject(this); //Entity.Destroy(ID);
                }
            }
        }
    }
}
