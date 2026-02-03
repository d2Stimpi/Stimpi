using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

// TODO: Conform to following C# coding style convention:
// https://learn.microsoft.com/en-us/dotnet/csharp/fundamentals/coding-style/identifier-names

namespace Stimpi
{
    public class Entity
    {
        private readonly bool _debug = true;
        // Entity can hold either valid ID (Instantiated Object) or Prefab Asset Handle (Not Instantiated Object)
        public readonly uint ID;
        public readonly ulong PrefabHandle;

        internal Entity(uint id)
        {
            ID = id;
            PrefabHandle = 0;
        }

        internal Entity(ulong prefabHandle)
        {
            PrefabHandle = prefabHandle;
        }

        protected Entity()
        {
            ID = 0;
            PrefabHandle = 0;
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, componentType);
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            T component = new T() { Entity = this };
            return component;
        }

        public T AddComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>())
                return null;

            Type componentType = typeof(T);
            if (!InternalCalls.Entity_AddComponent(ID, componentType))
            {
                if (_debug) Console.WriteLine($"Failed adding component {componentType.Name} on Entity {ID}");
                return null;
            }

            T component = new T() { Entity = this };
            return component;
        }

        public void RemoveComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return;

            Type componentType = typeof(T);
            InternalCalls.Entity_RemoveComponent(ID, componentType);
        }

        public Entity FindEntityByName(string name)
        {
            uint entityID = InternalCalls.Entity_FindEntityByName(name);
            if (entityID != 0)
                return new Entity(entityID);

            return null;
        }

        static public Entity[] FindAllEntitiesByName(string name)
        {
            return InternalCalls.Entity_FindAllEntitiesByName(name);
        }

        static public Entity FindByName(string name)
        {
            uint entityID = InternalCalls.Entity_FindEntityByName(name);
            if (entityID != 0)
                return new Entity(entityID);

            return null;
        }

        public Entity FindEntityByID(uint entityID)
        {
            bool validEntity = InternalCalls.Entity_IsValidEntityID(entityID);
            if (validEntity)
                return new Entity(entityID);

            return null;
        }

        public T As<T>() where T : Entity, new()
        {
            object instance = InternalCalls.GetScriptInstace(ID);
            if (instance is T)
                return instance as T;

            return null;
        }

        public Entity Instantiate(Entity entity, Vector2 position, float rotation)
        {
            if (entity != null)
            {
                // Handle instantiating of an Entity - make a copy
                if (entity.ID != 0)
                {
                    uint newEntityID = InternalCalls.Entity_Instantiate(entity.ID, ref position, ref rotation);
                    if (newEntityID != 0)
                        return new Entity(newEntityID);
                }
                // Handle instantiating a Prefab asset
                if (entity.PrefabHandle != 0)
                {
                    uint newEntityID = InternalCalls.Entity_InstantiatePrefab(entity.PrefabHandle, ref position, ref rotation);
                    if (newEntityID != 0)
                        return new Entity(newEntityID);
                }
            }

            return null;
        }

        // TODO: consider moving to EntityManager class
        public static T Create<T>() where T : Entity, new()
        {
            object instance = InternalCalls.CreateScriptInstance(typeof(T).FullName);
            if (instance is T)
                return instance as T;

            return null;
        }

        // TODO: consider moving to EntityManager class
        public static bool Destroy(uint ID)
        {
            return InternalCalls.Entity_Remove(ID);
        }

        public virtual void OnCreate()
        {
            Console.WriteLine("OnCreate Entity");
        }

        public virtual void OnUpdate(float ts)
        {
            if (_debug) Console.WriteLine($"OnUpdate Entity - time step: {ts}");
        }

        public virtual void OnCollisionBegin(Collision collision)
        {
            if (_debug) Console.WriteLine($"OnCollision Begin - entities: {ID}, {collision.OtherID}");
        }

        public virtual bool OnCollisionPreSolve(Collision collision)
        {
            if (_debug) Console.WriteLine($"OnCollision PreSolve - entities: {ID}, {collision.OtherID}");
            return true;
        }

        public virtual void OnCollisionPostSolve(Collision collision)
        {
            if (_debug) Console.WriteLine($"OnCollision PostSolve - entities: {ID}, {collision.OtherID}");
        }

        public virtual void OnCollisionEnd(Collision collision)
        {
            if (_debug) Console.WriteLine($"OnCollision End - entities: {ID}, {collision.OtherID}");
        }
    }
}
