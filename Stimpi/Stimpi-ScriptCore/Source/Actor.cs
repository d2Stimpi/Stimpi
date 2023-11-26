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
        public void OnCreate()
        {
            Console.WriteLine("OnCreate Actor");
        }

        public void OnUpdate(float ts)
        {
            Console.WriteLine($"OnUpdate Actor - time step: {ts}");
        }
    }
}
