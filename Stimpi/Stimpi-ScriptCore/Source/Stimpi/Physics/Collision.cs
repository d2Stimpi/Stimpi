using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    public struct ContactPoint
    {
        public readonly Vector2 point;

        public ContactPoint(Vector2 p)
        {
            point = p;
        }
    }

    public struct Collision
    {
        public uint OwnerID;    // To mirror C++ struct layout
        public uint EntityID;   // ID of colliding Entity

        public ContactPoint GetContacts()
        {
            InternalCalls.Collision_GetContacts(out ContactPoint contact);
            return contact;
        }
    }
}
