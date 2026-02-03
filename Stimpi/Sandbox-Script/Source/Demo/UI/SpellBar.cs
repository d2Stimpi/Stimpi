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
        private uint _iconsCount = 4;
        private Icon[] _icons = new Icon[4];
        private uint _elementCount = 0;

        // Exposed to Editor for placing on screen
        public Entity IconNorth;
        public Entity IconWest;
        public Entity IconEast;
        public Entity IconSputh;

        public QuadComponent QuadSample;

        // Key stroke tracking
        private string _keysRegisterString = "";
        private KeyCode _lastKeyPressed = KeyCode.KEY_UNKNOWN;

        public void UpdateIconsVisibility(uint visibleCount)
        {
            foreach (var icon in _icons)
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
                _icons[0] = IconNorth.As<Icon>();

            if (IconWest != null)
                _icons[1] = IconWest.As<Icon>();

            if (IconEast != null)
                _icons[2] = IconEast.As<Icon>();

            if (IconSputh != null)
                _icons[3] = IconSputh.As<Icon>();

            Console.WriteLine($"North Icon: {_icons[0]}");
            Console.WriteLine($"West Icon: {_icons[1]}");
            Console.WriteLine($"East Icon: {_icons[2]}");
            Console.WriteLine($"South Icon: {_icons[3]}");
        }

        public override void OnUpdate(float ts)
        {
            if (Input.IsKeyDown(KeyCode.KEY_Q) ||
                Input.IsKeyDown(KeyCode.KEY_W) ||
                Input.IsKeyDown(KeyCode.KEY_E) ||
                Input.IsKeyDown(KeyCode.KEY_R))
            {
                if (_elementCount < _iconsCount)
                { 
                    _elementCount++;

                    // Set element color (later texture)
                    if (Input.IsKeyDown(KeyCode.KEY_Q))
                    {
                        _lastKeyPressed = KeyCode.KEY_Q;
                        _icons[_elementCount - 1].SetColor(KeyToColorDict.Values[KeyCode.KEY_Q]);
                        RegisterKeyPress('Q');
                    }
                    if (Input.IsKeyDown(KeyCode.KEY_W))
                    {
                        _lastKeyPressed = KeyCode.KEY_W;
                        _icons[_elementCount - 1].SetColor(KeyToColorDict.Values[KeyCode.KEY_W]);
                        RegisterKeyPress('W');
                    }
                    if (Input.IsKeyDown(KeyCode.KEY_E))
                    {
                        _lastKeyPressed = KeyCode.KEY_E;
                        _icons[_elementCount - 1].SetColor(KeyToColorDict.Values[KeyCode.KEY_E]);
                        RegisterKeyPress('E');
                    }
                    if (Input.IsKeyDown(KeyCode.KEY_R))
                    {
                        _lastKeyPressed = KeyCode.KEY_R;
                        _icons[_elementCount - 1].SetColor(KeyToColorDict.Values[KeyCode.KEY_R]);
                        RegisterKeyPress('R');
                    }
                }

                UpdateIconsVisibility(_elementCount);
            }
        }

        /* Private methods */

        private void RegisterKeyPress(Char keyChr)
        {
            _keysRegisterString += keyChr;
        }

        public string ConsumeRegiseredKeyPattern()
        {
            string ret = _keysRegisterString;
            //Console.WriteLine("Registered key pattern str: " + _keysRegisterString);

            _elementCount = 0;
            UpdateIconsVisibility(_elementCount);

            // Clear and prepare for new key pattern
            _keysRegisterString = "";

            return ret;
        }
    }
}
