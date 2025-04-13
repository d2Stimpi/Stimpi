using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{
    /*
     * Class or struct to build a tree like structure where elements are linked by distance.
     *  - Each element is linked to the closest nearby element
     *  - Each frame the links are updated by recalculating the distances
     *  
     *  * Use a grid of buckets to hold nearby objects for performance
     */
    public class EnemyTracker
    {
        static private Grid<Enemy> s_GridEnemies = new Grid<Enemy>(new Vector2(128.0f, 128.0f));
        
        static private List<Enemy> s_Enemies = new List<Enemy>();

        static private void OnCreateAction(Enemy enemy)
        {
            s_Enemies.Add(enemy);
            Console.WriteLine($"OnGetAction: enemy {enemy.ID} added.");
        }

        static private void OnReleaseAction(Enemy enemy)
        {
            s_GridEnemies.Remove(enemy);

            s_Enemies.Remove(enemy);
            Console.WriteLine($"OnReleaseAction: enemy {enemy.ID} removed.");
        }

        static public void Initialize()
        {
            EnemyPool.RegisterOnGetAction(OnCreateAction);
            EnemyPool.RegisterOnReleaseAction(OnReleaseAction);
        }

        static public void AddToGrid(Enemy enemy)
        {
            s_GridEnemies.Add(enemy.GetPosition(), enemy);
        }

        static public void Update(Enemy enemy)
        {
            s_GridEnemies.Update(enemy.GetPosition(), enemy);
        }

        static public Enemy GetClosestEnemy(Vector2 pos)
        {
            Enemy closestEnemy = null;
            float distance = -1.0f;
            var enemies = EnemyPool.GetActiveEnemies();

            foreach (var enemy in enemies)
            {
                if (enemy.IsActive())
                {
                    Vector2 enemyPos = enemy.GetComponent<QuadComponent>().Position;
                    float enemyDistance = (pos - enemyPos).Length;
                    if (enemyDistance < distance || distance == -1.0f)
                    {
                        distance = enemyDistance;
                        closestEnemy = enemy;
                    }
                }
            }

            return closestEnemy;
        }

        public static void LogStuff()
        {
            s_GridEnemies.LogGridData();
        }
    }
}
