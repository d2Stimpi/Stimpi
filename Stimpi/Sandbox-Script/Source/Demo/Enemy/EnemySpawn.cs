﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{

    public class EnemySpawn : Entity
    {
        private QuadComponent _quad;
        private Vector2 _spawnLocation;

        public float SpawnFrequency = 2.0f;     // Spawn unit every SpawnFrequency seconds
        private float _timeTracker = 0;

        public float MaxEnemyCount = 2.0f;
        private int _enemyCount = 0;

        public override void OnCreate()
        {
            _quad = GetComponent<QuadComponent>();
            if (_quad != null)
                _spawnLocation = _quad.Position;
            else
                _spawnLocation = Vector2.Zero;
        }

        public override void OnUpdate(float ts)
        {
            _timeTracker += ts;
            if (_timeTracker >= SpawnFrequency)
            {
                _timeTracker = 0;
                SpawnEnemy();
            }
        }

        public void SpawnEnemy()
        {
            if (_enemyCount < MaxEnemyCount)
            {
                Enemy enemy = EnemyPool.GetObject();
                enemy.Initialize(this, _spawnLocation);

                // Add only initialized obj to Grid - with correct position
                EnemyTracker.AddToGrid(enemy);

                _enemyCount++;
            }
        }

        public void EnemyKilled()
        {
            _enemyCount--;
        }
    }
}
