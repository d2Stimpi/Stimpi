using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stimpi
{
    public struct Color
    {
        public float R;
        public float G;
        public float B;
        public float A;

        public Color(float r, float g, float b, float a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }

        public static Color Black { get { return new Color(0.0f, 0.0f, 0.0f, 1.0f); } }
        public static Color White { get { return new Color(1.0f, 1.0f, 1.0f, 1.0f); } }

        public override string ToString()
        {
            return $"Color [R:{R}, G:{G}, B:{B}, A:{A}]";
        }
    }
}
