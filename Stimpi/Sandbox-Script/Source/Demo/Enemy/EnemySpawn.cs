using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{
    public class EnemySpawn : Entity
    {
        private QuadComponent Quad;
        private Vector2 SpawnLocation;

        public float SpawnFrequency = 2.0f;     // Spawn unit every SpawnFrequency seconds
        private float TimeTracker = 0;

        public float MaxEnemyCount = 25;
        private int EnemyCount = 0;

        public override void OnCreate()
        {
            Quad = GetComponent<QuadComponent>();
            if (Quad != null)
                SpawnLocation = Quad.Position;
            else
                SpawnLocation = Vector2.Zero;
        }

        public override void OnUpdate(float ts)
        {
            TimeTracker += ts;
            if (TimeTracker >= SpawnFrequency)
            {
                TimeTracker = 0;
                SpawnEnemy();
            }
        }

        public void SpawnEnemy()
        {
            if (EnemyCount < MaxEnemyCount)
            {
                Enemy enemy = Entity.Create<Enemy>();
                enemy.Initialize(this, SpawnLocation);

                EnemyCount++;
            }
        }

        public void EnemyDestroyed()
        {
            EnemyCount--;
        }
    }
}
