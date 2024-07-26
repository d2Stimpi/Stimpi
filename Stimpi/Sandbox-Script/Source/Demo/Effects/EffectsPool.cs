using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

// TODO: Manage non-Entity classes, especially static ones in Mono ScriptEngine.
//       When Scene restarts, ObjectPool data is not explicitly deleted from Mono runtime

namespace Demo
{
    public static class EffectsPool
    {
        private static readonly bool _debug = true;
        private static ObjectPool<Explosion> s_explosionPool = new ObjectPool<Explosion>(10, EffectsPool.CreateExplosionObject);

        // Pool object create method
        public static Explosion CreateExplosionObject()
        {
            Explosion explosion = Entity.Create<Explosion>();
            if (explosion == null)
                Console.WriteLine("Failed to create new Explosion object");

            if (_debug) Console.WriteLine("New Explosion object created");
            return explosion;
        }

        public static Explosion GetObject()
        {
            if (_debug) Console.WriteLine($"EffectsPool size {s_explosionPool.GetCount()}");
            return s_explosionPool.GetObject();
        }

        public static void ReleaseObject(Explosion explosion)
        {
            s_explosionPool.ReleaseObject(explosion);
        }

        public static void Clear()
        {
            s_explosionPool.Clear();
        }
    }
}
