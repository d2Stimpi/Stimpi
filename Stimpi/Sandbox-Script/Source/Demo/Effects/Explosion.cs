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
        QuadComponent _quad;
        AnimatedSpriteComponent _anim;


        private bool _played = false;

        public void Initialize(Vector2 position)
        {
            _quad.Position = position;
            Random rnd = new Random();
            _quad.Rotation = rnd.Next(0, 3);
            _played = false;
        }

        public void Play()
        {
            _anim.Play("explosion01.anim");
            _played = true;
        }

        public override void OnCreate()
        {
            _quad = AddComponent<QuadComponent>();
            _quad.Size = new Vector2(12.0f, 12.0f);

            _anim = AddComponent<AnimatedSpriteComponent>();
            _anim.AddAnimation("animations\\explosion01.anim");
        }

        public override void OnUpdate(float ts)
        {
            if (_played)
            {
                if (_anim.AnimState == AnimationState.COMPELETED)
                {
                    _played = false;                    
                    EffectsPool.ReleaseObject(this); //Entity.Destroy(ID);
                }
            }
        }
    }
}
