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

        public override void OnCreate()
        {
            Console.WriteLine($"OnCreate: {ID}");
        }

        public override void OnUpdate(float ts)
        {

        }
    }
}
