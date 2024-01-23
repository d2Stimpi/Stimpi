using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    public class Input
    {
        public static bool IsKeyPressed(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyPressed(keycode);
        }
    }
}
