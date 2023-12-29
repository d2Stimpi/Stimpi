using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    public struct Collision
    {
        public uint OwnerID;    // To mirror C++ struct layout
        public uint EntityID;   // ID of colliding Entity
    }
}
