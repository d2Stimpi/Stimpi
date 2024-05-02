using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    public struct Vector2
    {
        public float X;
        public float Y;

        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }

        public static Vector2 Zero { get { return new Vector2(0.0f, 0.0f); } }
        public static Vector2 One { get { return new Vector2(1.0f, 1.0f); } }

        public override string ToString()
        {
            return $"vec2 [{X}, {Y}]";
        }

        public static Vector2 operator+ (Vector2 v1, Vector2 v2)
        {
            return new Vector2(v1.X + v2.X, v1.Y + v2.Y);
        }

        public static Vector2 operator +(Vector2 v1, float f)
        {
            return new Vector2(v1.X + f, v1.Y + f);
        }

        public static Vector2 operator- (Vector2 v1, Vector2 v2)
        {
            return new Vector2(v1.X - v2.X, v1.Y - v2.Y);
        }

        public static Vector2 operator* (Vector2 v1, float f)
        {
            return new Vector2(v1.X * f, v1.Y * f);
        }
    }
}
