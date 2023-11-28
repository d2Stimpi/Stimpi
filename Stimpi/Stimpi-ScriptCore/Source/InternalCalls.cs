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
        /* Components */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Entity_HasComponent(uint entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Entity_CreateComponent(uint entityID, Type componentType);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Entity_RemoveComponent(uint entityID, Type componentType);

        /* QuadComponent */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool QuadComponent_GetPosition(uint entityID, out Vector2 quad);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool QuadComponent_SetPosition(uint entityID, ref Vector2 quad);

        /* Input */
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static bool Input_IsKeyPressed(KeyCode keycode);
    }
}
