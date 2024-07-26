using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    public class ObjectPool<T> where T : new()
    {
        private List<T> _objectsList = new List<T>();
        private uint _counter = 0;
        private uint _capacity;
        // Custom, supplied initializer method for creating Pool Objects
        private Func<T> _initializer = null;

        public ObjectPool(uint capacity, Func<T> initializer)
        {
            _capacity = capacity;
            _initializer = initializer;
        }

        public uint GetCount()
        {
            return _counter;
        }

        public T GetObject()
        {
            T objectItem;

            if (_counter > 0)
            {
                objectItem = _objectsList[0];
                _objectsList.RemoveAt(0);
                _counter--;
            }
            else
            {
                objectItem = _initializer();
            }

            return objectItem;
        }

        public void ReleaseObject(T item)
        {
            if (_counter < _capacity)
            {
                _objectsList.Add(item);
                _counter++;
            }
        }

        public void Clear()
        {
            _objectsList.Clear();
            _counter = 0;
        }
    }
}
