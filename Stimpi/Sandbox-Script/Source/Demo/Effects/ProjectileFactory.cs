﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{
    public enum ProjectileType
    { 
        FIREBALL = 0,
        LIGHTNING_BOLT
    }

    public struct ProjSpawnParams
    {
        public readonly Entity OwnerEntity;
        public readonly Vector2 Size;
        public readonly Vector2 TargetPosition;

        public ProjSpawnParams(Entity owner, Vector2 targetPos, Vector2 size)
        {
            OwnerEntity = owner;
            TargetPosition = targetPos;
            Size = size;
        }
    }

    public static class ProjectileFactory
    {
        private static readonly bool s_debug = true;
        private static ObjectPool<Projectile> s_projectilePool = new ObjectPool<Projectile>(200, CreateProjectileObject);

        /* Create Object method */

        public static Projectile CreateProjectileObject()
        {
            Projectile projectile = Entity.Create<Projectile>();
            if (projectile == null)
                Console.WriteLine("Failed to create new Projectile object");

            if (s_debug) Console.WriteLine("New Projectile object created");
            return projectile;
        }

        /* Actual Factory create method */

        public static Projectile CreateProjectile(ProjectileType type, ProjSpawnParams param)
        {
            //Projectile projectile = CreateProjectileObject();
            if (s_debug) Console.WriteLine($"Projectile pool size {s_projectilePool.GetCount()}");
            Projectile projectile = s_projectilePool.GetObject();

            // Init projectile based on Type requested
            // - Animation to use and its params (Looping, etc...)
            // - Velocity
            // - Damage and other data

            projectile.Initialize(type, param.OwnerEntity, param.TargetPosition, param.Size);

            switch(type)
            {
                case ProjectileType.FIREBALL:
                    projectile.SetAndPlayAnimation("fireball01.anim", true);
                    break;
                case ProjectileType.LIGHTNING_BOLT:
                    projectile.SetAndPlayAnimation("lightning.anim", false);
                    projectile.SetAnimationPlaybackSpeed(3.0f);
                    projectile.Velocity = 50.0f;
                    break;
                default:
                    projectile.SetAndPlayAnimation("fireball01.anim", true);
                    break;
            }
            

            return projectile;
        }

        public static void Release(Projectile proj)
        {
            if (s_debug) Console.WriteLine($"Release Projectile");
            // Move it far away out of sight :D
            proj.Invalidate(new Vector2(-1000.0f, -1000.0f));

            s_projectilePool.ReleaseObject(proj);
        }

        public static void Clear()
        {
            s_projectilePool.Clear();
        }
    }
}
