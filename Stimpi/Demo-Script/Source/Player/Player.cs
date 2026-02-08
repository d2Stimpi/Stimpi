using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Player
{
    public class Player : Entity
    {
        public QuadComponent ProjectileObject;
        public float Offset = 2.0f;
        public override void OnCreate()
        {
            AnimatedSpriteComponent anim = GetComponent<AnimatedSpriteComponent>();
            if (anim != null )
            {
                anim.Play();
                anim.Looping = true;
            }
        }

        public override void OnUpdate(float ts)
        {
            if (Input.IsKeyPressed(KeyCode.KEY_SPACE))
            {
                Vector2 pos = GetComponent<QuadComponent>().Position;
                pos.X += Offset;
                Entity proj = Instantiate(ProjectileObject.Entity, pos, 0.0f);
                Physics.ApplyForceCenter(proj.ID, new Vector2(10.0f, 3.0f), false);
            }
        }
    }
}
