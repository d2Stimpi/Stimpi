using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    public class Contact
    {
        public Vector2 Point;
        public Vector2 ImpactVelocty;

        public Contact()
        {
            Point = new Vector2(0.0f, 0.0f);
        }
    }

    public struct Collision
    {
        public uint OwnerID;            // To mirror C++ struct layout
        public uint OtherID;            // ID of colliding Entity

        public Vector2 ImpactVelocty
        {
            get
            {
                InternalCalls.Collision_GetImpactVelocity(OwnerID, OtherID, out Vector2 velocity);
                return velocity;
            }
        }

        public Contact[] Contacts
        {
            get
            {
                return InternalCalls.Collision_GetContacts(OwnerID, OtherID);
            }
        }
    }
}
