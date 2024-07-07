using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    public class Physics
    {
        public static bool ApplyForce(uint entityID, Vector2 force, Vector2 point, bool wake)
        {
            return InternalCalls.Physics_ApplyForce(entityID, ref force, ref point, wake);
        }

        public static bool ApplyForceCenter(uint entityID, Vector2 force, bool wake)
        {
            return InternalCalls.Physics_ApplyForceCenter(entityID, ref force, wake);
        }

        public static bool ApplyImpulse(uint entityID, Vector2 impulse, Vector2 point, bool wake)
        {
            return InternalCalls.Physics_ApplyLinearImpulse(entityID, ref impulse, ref point, wake);
        }

        public static bool ApplyImpulseCenter(uint entityID, Vector2 impulse, bool wake)
        {
            return InternalCalls.Physics_ApplyLinearImpulseCenter(entityID, ref impulse, wake);
        }

        public static bool GetLinearVelocity(uint entityID, out Vector2 velocity)
        {
            bool result = InternalCalls.Physics_GetLinearVelocity(entityID, out velocity);
            if (!result)
                Console.WriteLine($"GetLinearVelocity failed - no RigidBody2DComponent component on entity {entityID}");

            return result;
        }

        public static bool SetLinearVelocity(uint entityID, Vector2 velocity)
        {
            bool result = InternalCalls.Physics_SetLinearVelocity(entityID, ref velocity);
            if (!result)
                Console.WriteLine($"SetLinearVelocity failed - no RigidBody2DComponent component on entity {entityID}");

            return result;
        }
    }
}
