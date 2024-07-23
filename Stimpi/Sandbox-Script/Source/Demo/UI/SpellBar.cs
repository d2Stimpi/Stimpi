using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{
    
    static class KeyToColorDict
    {
        public static Dictionary<KeyCode, Color> Values = new Dictionary<KeyCode, Color>()
        {
            { KeyCode.KEY_Q, new Color(0.8f, 0.25f, 0.25f, 1.0f) },
            { KeyCode.KEY_W, new Color(0.25f, 0.8f, 0.25f, 1.0f) },
            { KeyCode.KEY_E, new Color(0.25f, 0.25f, 0.8f, 1.0f) },
            { KeyCode.KEY_R, new Color(0.8f, 0.8f, 0.25f, 1.0f) }
        };
    }
    


    public class SpellBar : Entity
    {
        // Convenience access to Icons
        private uint IconsCount = 4;
        private Icon[] Icons = new Icon[4];
        private uint ElementCount = 0;

        // Exposed to Editor for placing on screen
        public Entity IconNorth;
        public Entity IconWest;
        public Entity IconEast;
        public Entity IconSputh;


        public void UpdateIconsVisibility(uint visibleCount)
        {
            foreach (var icon in Icons)
            {
                if (visibleCount > 0)
                {
                    icon.Enable();
                    visibleCount--;
                }
                else
                {
                    icon.Disable();
                }
            }
        }

        public override void OnCreate()
        {
            if (IconNorth != null)
                Icons[0] = IconNorth.As<Icon>();

            if (IconWest != null)
                Icons[1] = IconWest.As<Icon>();

            if (IconEast != null)
                Icons[2] = IconEast.As<Icon>();

            if (IconSputh != null)
                Icons[3] = IconSputh.As<Icon>();

            Console.WriteLine($"North Icon: {Icons[0]}");
            Console.WriteLine($"West Icon: {Icons[1]}");
            Console.WriteLine($"East Icon: {Icons[2]}");
            Console.WriteLine($"South Icon: {Icons[3]}");
        }

        public override void OnUpdate(float ts)
        {
            if (Input.IsKeyDown(KeyCode.KEY_Q) ||
                Input.IsKeyDown(KeyCode.KEY_W) ||
                Input.IsKeyDown(KeyCode.KEY_E) ||
                Input.IsKeyDown(KeyCode.KEY_R))
            {
                if (ElementCount < IconsCount)
                { 
                    ElementCount++;

                    // Set element color (later texture)
                    if (Input.IsKeyDown(KeyCode.KEY_Q))
                        Icons[ElementCount - 1].SetColor(KeyToColorDict.Values[KeyCode.KEY_Q]);
                    if (Input.IsKeyDown(KeyCode.KEY_W))
                        Icons[ElementCount - 1].SetColor(KeyToColorDict.Values[KeyCode.KEY_W]);
                    if (Input.IsKeyDown(KeyCode.KEY_E))
                        Icons[ElementCount - 1].SetColor(KeyToColorDict.Values[KeyCode.KEY_E]);
                    if (Input.IsKeyDown(KeyCode.KEY_R))
                        Icons[ElementCount - 1].SetColor(KeyToColorDict.Values[KeyCode.KEY_R]);
                }

                UpdateIconsVisibility(ElementCount);
            }

            if (Input.IsKeyDown(KeyCode.KEY_SPACE))
            {
                ElementCount = 0;
                UpdateIconsVisibility(ElementCount);
            }
        }
    }
}
