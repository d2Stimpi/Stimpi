using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace DemoScript
{
    public class Projectile : Entity
    {


        public override void OnCreate()
        {
            AnimatedSpriteComponent anim = GetComponent<AnimatedSpriteComponent>();
            if (anim != null)
            {
                anim.Looping = true;
                anim.Play();
            }
        }

        public override void OnUpdate(float ts)
        {
        }

        public override void OnCollisionBegin(Collision collision)
        {
        }
    }
}
