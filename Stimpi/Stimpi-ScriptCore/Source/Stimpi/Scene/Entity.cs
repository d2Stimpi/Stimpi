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
        public readonly uint ID;

        internal Entity(uint id)
        {
            ID = id;
            //Console.WriteLine($"Created Entity with id {ID}");
        }

        protected Entity()
        {
            ID = 0;
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
                return null;

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

        // TODO: move to EntityManager class
        public static T Create<T>() where T : Entity, new()
        {
            object instance = InternalCalls.CreateScriptInstance(typeof(T).FullName);
            if (instance is T)
                return instance as T;

            return null;
        }

        // TODO: move to EntityManager class
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
            Console.WriteLine($"OnUpdate Entity - time step: {ts}");
        }

        public virtual void OnCollisionBegin(Collision collision)
        {
            Console.WriteLine($"OnCollision Begin - entities: {ID}, {collision.OtherID}");
        }

        public virtual void OnCollisionEnd(Collision collision)
        {
            Console.WriteLine($"OnCollision End - entities: {ID}, {collision.OtherID}");
        }
    }
}
