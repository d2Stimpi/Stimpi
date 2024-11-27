using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    public sealed class MathUtils
    {
        public static Vector2 RotatePointAroundCenter(Vector2 point, Vector2 center, double rad)
        {
            float x1 = (point.X - center.X) * (float)Math.Cos(rad) - (point.Y - center.Y) * (float)Math.Sin(rad) + center.X;
            float y1 = (point.X - center.X) * (float)Math.Sin(rad) + (point.Y - center.Y) * (float)Math.Cos(rad) + center.Y;

            return new Vector2(x1, y1);
        }
    }
}
