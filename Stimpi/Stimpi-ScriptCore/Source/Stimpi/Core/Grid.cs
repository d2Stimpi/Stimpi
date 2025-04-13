using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    internal class Bucket<T> where T : new()
    {
        private List<T> _objects = new List<T>();

        public List<T> Objects { get => _objects; }
    }

    internal struct Index
    {
        public int X;
        public int Y;

        public Index (int x, int y)
        {
            X = x;
            Y = y;
        }

        public bool Equals(Index index)
        {
            return index.X == X && index.Y == Y;
        }
    }

    /*
     * Class that represents object container that acts as a Grid.
     * Grid is endless, bucket index is calculated based on the bucket size.
     * Only buckets with objects exist.
     * 
     * Grid should be updated each frame by remove-adding the objects that are moving.
     * 
     * Add: object is added to the grid based on Vector2 position provided.
     * Remove: object is removed based on internal index lookup dictionary.
     */

    public class Grid<T> where T : new()
    {
        public readonly Vector2 BucketSize;

        private Dictionary<Index, Bucket<T>> _buckets = new Dictionary<Index, Bucket<T>>();
        // for internal keeping track of Object - Index for faster removal
        private Dictionary<T, Index> _indexDict = new Dictionary<T, Index>();

        public Grid(Vector2 bucketSize)
        {
            BucketSize = bucketSize;
        }

        public void Add(Vector2 pos, T obj)
        {
            // Find out the Bucket id based on position and bucket size
            Index index = new Index((int)Math.Floor(pos.X / BucketSize.X), (int)Math.Floor(pos.Y / BucketSize.Y));

            if (_buckets.ContainsKey(index))
            {
                // Bucket already exists at the index position, add the object if !Contains
                Console.WriteLine($"[Grid::Add] adding obj to bucket: {obj.ToString()} with pos: {pos}");
                Console.WriteLine($"[Grid::Add] adding bucket to dict at index: {index.X}, {index.Y}");
                Bucket<T> bucket = _buckets[index];
                if (!bucket.Objects.Contains(obj))
                {
                    bucket.Objects.Add(obj);
                    _indexDict.Add(obj, index);
                }
            }
            else
            {
                Console.WriteLine($"[Grid::Add] adding obj to bucket: {obj.ToString()} with pos: {pos}");
                Console.WriteLine($"[Grid::Add] adding bucket to dict at index: {index.X}, {index.Y}");
                Bucket<T> newBucket = new Bucket<T>();
                newBucket.Objects.Add(obj);
                _buckets.Add(index, newBucket);
                _indexDict.Add(obj, index);
            }
        }

        /*
         * Remove the object from the bucket specified by the world position of that object.
         * Specific usage is assumed.
         */
        public bool Remove(T obj)
        {
            bool removed = false;
            
            // Check if object is register in index lookup dictionary
            if (_indexDict.ContainsKey(obj))
            {
                Index index = _indexDict[obj];

                if (_buckets.ContainsKey(index))
                {
                    removed = _buckets[index].Objects.Remove(obj);
                    _indexDict.Remove(obj);
                }
            }

            return removed;
        }

        public bool Update(Vector2 pos, T obj)
        {
            bool changedBucket = false;

            Index checkIndex = new Index((int)Math.Floor(pos.X / BucketSize.X), (int)Math.Floor(pos.Y / BucketSize.Y));
            if (_indexDict.ContainsKey(obj))
            {
                Index index = _indexDict[obj];
                changedBucket = index.Equals(checkIndex);

                if (!changedBucket)
                {
                    Remove(obj);
                    Add(pos, obj);
                }
            }

            return changedBucket;
        }

        public List<T> GetObjects(Vector2 pos)
        {
            return null;
        }

        public void LogGridData()
        {
            foreach (KeyValuePair<Index, Bucket<T>> pair in _buckets)
            {
                Console.WriteLine($"***** Index: {pair.Key.X}, {pair.Key.Y} *****");
                foreach (T obj in pair.Value.Objects)
                {
                    Console.WriteLine("== Obj: " + obj.ToString());
                }
                Console.WriteLine("**************************************");
            }
        }
    }
}
