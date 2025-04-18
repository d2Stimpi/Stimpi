using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    internal class Bucket<T> where T : class, new()
    {
        private List<T> _objects = new List<T>();

        public List<T> Objects { get => _objects; }
    }

    public struct Index
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

    public class Grid<T> where T : class, new()
    {
        public readonly Vector2 BucketSize;

        private Dictionary<Index, Bucket<T>> _buckets = new Dictionary<Index, Bucket<T>>();
        // for internal keeping track of Object - Index for faster removal
        private Dictionary<T, Index> _indexDict = new Dictionary<T, Index>();
        // for tracking objects to skip when looking fro next closest element
        private HashSet<T> _skipSet = new HashSet<T>();

        public Grid(Vector2 bucketSize)
        {
            BucketSize = bucketSize;
        }

        public void Reset()
        {
            _buckets.Clear();
            _indexDict.Clear();
            _skipSet.Clear();
        }

        public void Add(Vector2 pos, T obj)
        {
            // Find out the Bucket id based on position and bucket size
            Index index = GetBucketIndex(pos);

            if (_buckets.ContainsKey(index))
            {
                // Bucket already exists at the index position, add the object if !Contains
                Bucket<T> bucket = _buckets[index];
                if (!bucket.Objects.Contains(obj))
                {
                    bucket.Objects.Add(obj);
                    _indexDict.Add(obj, index);
                }
            }
            else
            {
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

                    // Remove the bucket if it is empty
                    if (_buckets[index].Objects.Count == 0)
                        _buckets.Remove(index);
                }
            }

            return removed;
        }

        public bool Update(Vector2 pos, T obj)
        {
            bool changedBucket = false;

            Index checkIndex = GetBucketIndex(pos);
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

        /*
         *  Utils and logging functions
         */

        public Index GetBucketIndex(Vector2 pos)
        {
            return new Index((int)Math.Floor(pos.X / BucketSize.X), (int)Math.Floor(pos.Y / BucketSize.Y));
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

        /*
         *  Methods for searching data
         */

        public List<Index> GetBucketsInRange(Vector2 pos, float radius)
        {
            List<Index> found = new List<Index>();
            
            Index center = GetBucketIndex(pos);

            int startX = center.X;
            int endX = center.X;
            int startY = center.Y;
            int endY = center.Y;

            if (radius != 0)
            {
                Index bottomLeft = GetBucketIndex(pos - radius);
                Index topRight = GetBucketIndex(pos + radius);
                startX = bottomLeft.X;
                endX = topRight.X;
                startY = bottomLeft.Y;
                endY = topRight.Y;
            }

            // Find the buckets that exist in these ranges
            for (int i = startX; i <= endX; i++)
            {
                for (int j = startY; j <= endY; j++)
                {
                    Index key = new Index(i, j);
                    if (_buckets.ContainsKey(key))
                        found.Add(key);
                }
            }

            return found;
        }

        public T GetClosestObject(Vector2 pos, float radius, Func<Vector2, T, float> comparator)
        {
            T closest = null;
            float closestDistance = radius;

            List<Index> indexList = GetBucketsInRange(pos, radius);
            foreach (Index index in indexList)
            {
                //Console.WriteLine($"GetClosestObject - bucket visited: {index.X}, {index.Y}");
                Bucket<T> bucket = _buckets[index];
                // All buckets are assumed to be non-empty
                foreach (T obj in bucket.Objects)
                {
                    // Check if object should be skipped
                    if (_skipSet.Contains(obj))
                        continue;

                    float distance = comparator(pos, obj);
                    if (distance <= closestDistance)
                    {
                        closestDistance = distance;
                        closest = obj;
                    }
                }
            }

            return closest;
        }

        public void SkipAdd(T obj)
        {
            _skipSet.Add(obj);
        }

        public void SkipReset()
        {
            _skipSet.Clear();
        }
    }
}
