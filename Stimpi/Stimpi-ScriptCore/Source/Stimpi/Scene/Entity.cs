using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    public class Entity
    {
        public readonly uint ID;

        internal Entity(uint id)
        {
            ID = id;
            Console.WriteLine($"Created Entity with id {ID}");
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

        public T CreateComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>())
                return null;

            Type componentType = typeof(T);
            if (!InternalCalls.Entity_CreateComponent(ID, componentType))
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

        public virtual void OnCreate()
        {
            Console.WriteLine("OnCreate Entity");
        }

        public virtual void OnUpdate(float ts)
        {
            Console.WriteLine($"OnUpdate Entity - time step: {ts}");
        }
    }
}
