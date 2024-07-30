using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{
    class HealthBar : Entity
    {
        private Entity _owner;
        private QuadComponent _quad;
        private SpriteComponent _sprite;

        public Vector2 Offset = new Vector2(0.0f, 6.0f);

        public void Initialize(Entity owner)
        {
            _owner = owner;
            Vector2 ownerPos = owner.GetComponent<QuadComponent>().Position;
            _quad.Position = ownerPos + Offset;
        }

        public override void OnCreate()
        {
            _quad = AddComponent<QuadComponent>();
            _quad.Size = new Vector2(6.0f, 1.0f);

            _sprite = AddComponent<SpriteComponent>();
            _sprite.Color = new Color(0.85f, 0.35f, 0.25f, 1.0f);
        }

        public void UpdatePosition()
        {
            Vector2 ownerPos = _owner.GetComponent<QuadComponent>().Position;
            _quad.Position = ownerPos + Offset;
        }

        public void SetColor(Color color)
        {
            _sprite.Color = color;
        }

        public void SetFillPercentage(float fill)
        {
            if (fill > 1.0f) fill = 1.0f;
            if (fill < 0f) fill = 0f;

            _quad.Size = new Vector2(6.0f * fill, 1.0f);
            Offset.X = - (6.0f - (6.0f * fill));
        }

        public void SetDisabled(bool disabled)
        {
            _sprite.SetDisabled(disabled);
        }

        public void Destroy()
        {
            Entity.Destroy(ID);
        }
    }
}
