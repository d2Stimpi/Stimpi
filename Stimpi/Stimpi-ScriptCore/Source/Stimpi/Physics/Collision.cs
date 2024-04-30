using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    public class Contact
    {
        public Vector2 Point1;
        public Vector2 Point2;
        public uint PointCount;
        public Vector2 ImpactVelocty;

        public Contact()
        {
            Point1 = new Vector2(0.0f, 0.0f);
            Point2 = new Vector2(0.0f, 0.0f);
            PointCount = 0;
        }
    }

    public struct Collision
    {
        public uint OwnerID;    // To mirror C++ struct layout
        public uint OtherID;   // ID of colliding Entity

        public Contact[] Contacts
        {
            get
            {
                return InternalCalls.Collision_GetContacts(OwnerID, OtherID);
            }
        }
    }
}
