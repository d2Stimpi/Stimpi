using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace Stimpi
{
    public class InternalCalls
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void Sample();
    }
}
