using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime;

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
        public static Vector2 AxisX { get { return new Vector2(1.0f, 0.0f); } }
        public static Vector2 AxisY { get { return new Vector2(0.0f, 1.0f); } }

        public float Length { get { return (float)Math.Sqrt(X*X + Y*Y); } }
        public Vector2 Unit { get { return new Vector2(X/Length, Y/Length); } }
        public Vector2 Inv { get { return new Vector2(-X, -Y); } }

        public float Dot(Vector2 other) { return X * other.X + other.Y * Y; }
        public float Cross(Vector2 other) { return (float)Math.Sqrt(X * X + Y * Y) * (float)Math.Sqrt(other.X*other.X + other.Y*other.Y); }
        public float Angle(Vector2 other) { return (float)Math.Acos(Dot(other) / Cross(other)); }

        public float Distance(Vector2 other) { return (float)Math.Sqrt((other.X - X) * (other.X - X) + (other.Y - Y) * (other.Y - Y)); }

        public override string ToString()
        {
            return $"vec2 [{X}, {Y}]";
        }

        public static Vector2 operator+ (Vector2 v1, Vector2 v2)
        {
            return new Vector2(v1.X + v2.X, v1.Y + v2.Y);
        }

        public static Vector2 operator+ (Vector2 v1, float f)
        {
            return new Vector2(v1.X + f, v1.Y + f);
        }

        public static Vector2 operator- (Vector2 v1, Vector2 v2)
        {
            return new Vector2(v1.X - v2.X, v1.Y - v2.Y);
        }

        public static Vector2 operator- (Vector2 v1, float f)
        {
            return new Vector2(v1.X - f, v1.Y - f);
        }

        public static Vector2 operator* (Vector2 v1, float f)
        {
            return new Vector2(v1.X * f, v1.Y * f);
        }

        public static Vector2 operator/ (Vector2 v1, float f)
        {
            return new Vector2(v1.X / f, v1.Y / f);
        }

        public bool Equal(Vector2 v1)
        {
            return (X == v1.X && Y == v1.Y);
        }
    }
}
