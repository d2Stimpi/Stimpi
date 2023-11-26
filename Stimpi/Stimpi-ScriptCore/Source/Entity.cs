using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    class Entity
    {
        public float PublicFloatVar = 5.0f;

        public void PrintFloatVar()
        {
            Console.WriteLine("From C# - PublicFloatVar = {0:F}", PublicFloatVar);
        }

        public void DoSomeStuff(float param)
        {
            PublicFloatVar += param;
        }

        public void PrintMessage(String msg, float val)
        {
            Console.WriteLine(msg + " {0:F}", val);
        }

        public void CallNativeCode()
        {
            InternalCalls.Sample();
        }
    }
}
