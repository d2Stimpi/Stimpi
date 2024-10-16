using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{
    public static class EnemyPool
    {
        private static readonly bool s_debug = true;
        private static ObjectPool<Enemy> s_EnemyPool = new ObjectPool<Enemy>(20, EnemyPool.CreateEnemyObject);

        // Pool object create method
        public static Enemy CreateEnemyObject()
        {
            Enemy enemy = Entity.Create<Enemy>();
            if (enemy == null)
                Console.WriteLine("Failed to create new Enemy object");

            if (s_debug) Console.WriteLine("New Enemy object created");
            return enemy;
        }

        public static Enemy GetObject()
        {
            if (s_debug) Console.WriteLine($"EnemyPool size {s_EnemyPool.GetCount()}");
            return s_EnemyPool.GetObject();
        }

        public static void ReleaseObject(Enemy enemy)
        {
            s_EnemyPool.ReleaseObject(enemy);
        }

        public static void Clear()
        {
            s_EnemyPool.Clear();
        }
    }
}
