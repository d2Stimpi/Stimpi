using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    public class Input
    {
        /* Keyboard events */
        public static bool IsKeyDown(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyDown(keycode);
        }

        public static bool IsKeyPressed(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyPressed(keycode);
        }

        public static bool IsKeyUp(KeyCode keycode)
        {
            return InternalCalls.Input_IsKeyUp(keycode);
        }

        /* Mouse events */
        public static bool IsMouseDown(MouseCode code)
        {
            return InternalCalls.Input_IsMouseDown(code);
        }

        public static bool IsMousePressed(MouseCode code)
        {
            return InternalCalls.Input_IsMousePressed(code);
        }

        public static bool IsMouseUp(MouseCode code)
        {
            return InternalCalls.Input_IsMouseUp(code);
        }

        public static Vector2 GetMousePosition()
        {
            InternalCalls.Input_GetMousePosition(out Vector2 position);
            return position;
        }
    }
}
