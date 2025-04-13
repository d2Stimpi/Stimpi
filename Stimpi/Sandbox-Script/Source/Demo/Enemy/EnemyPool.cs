using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{
    using OnGetAction = Action<Enemy>;
    using OnReleaseAction = Action<Enemy>;

    public static class EnemyPool
    {
        private static readonly bool s_debug = false;
        private static ObjectPool<Enemy> s_EnemyPool = new ObjectPool<Enemy>(20, EnemyPool.CreateEnemyObject);

        // Listeners - Actions for Create/Release objects
        private static List<OnGetAction> s_OnCreateActions = new List<OnGetAction>();
        private static List<OnReleaseAction> s_OnReleaseActions = new List<OnReleaseAction>();

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
            Enemy enemy = s_EnemyPool.GetObject();

            if (enemy != null)
            {
                s_OnCreateActions.ForEach(action => action(enemy));
            }

            if (s_debug) Console.WriteLine($"EnemyPool size {s_EnemyPool.GetCount()}");
            return enemy;
        }

        public static void ReleaseObject(Enemy enemy)
        {
            s_OnReleaseActions.ForEach(action => action(enemy));
            s_EnemyPool.ReleaseObject(enemy);
        }

        public static void Clear()
        {
            s_EnemyPool.Clear();
        }

        public static Enemy[] GetActiveEnemies()
        {
            Entity[] entities = Entity.FindAllEntitiesByName("Demo.Enemy");
            return entities.Select(entity => entity.As<Enemy>()).ToArray();
        }

        public static void RegisterOnGetAction(OnGetAction action)
        {
            if (!s_OnCreateActions.Contains(action))
            {
                s_OnCreateActions.Add(action);
            }
        }

        public static void UnregisterOnGetAction(OnGetAction action)
        {
            s_OnCreateActions.Remove(action);
        }

        public static void RegisterOnReleaseAction(OnReleaseAction action)
        {
            if (!s_OnReleaseActions.Contains(action))
            {
                s_OnReleaseActions.Add(action);
            }
        }

        public static void UnregisterOnReleaseAction(OnReleaseAction action)
        {
            s_OnReleaseActions.Remove(action);
        }
    }
}
