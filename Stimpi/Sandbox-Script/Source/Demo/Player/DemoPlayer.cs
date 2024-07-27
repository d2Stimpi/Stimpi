using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

// TODO - script internal calls
// Get Window size or Camera view box

namespace Demo
{
    [ScriptOrder(Priority = 1)]
    public class DemoPlayer : Entity
    {
        private QuadComponent Quad;
        public float Velocity = 20.0f;

        public Entity Camera;
        public Entity Cursor;
        private QuadComponent CursorQuad;

        private SpellBar _spellBar;

        public override void OnCreate()
        {
            // FIXME: This is temporary solution, add OnDestroy to scripts
            // Clear all instances in ObjectPool(s) on "Scene Start"
            EffectsPool.Clear();
            ProjectileFactory.Clear();

            Quad = GetComponent<QuadComponent>();
            if (Cursor != null)
            {
                CursorQuad = Cursor.GetComponent<QuadComponent>();
            }
        }

        public override void OnUpdate(float ts)
        {
            // Grab spellBar ref, after all onCreates are done
            if (_spellBar == null)
            {
                var entity = FindEntityByName("UI_SpellBar");
                if (entity != null)
                {
                    _spellBar = entity.As<SpellBar>();
                    if (_spellBar == null)
                        Console.WriteLine("_spellBar failed to be found");
                    else
                        Console.WriteLine("_spellBar found");
                }
            }

            // Tracking "dummy" object - just to show where the target actually is (cursor pos)
            if (CursorQuad != null)
            {
                Vector2 mousePos = Input.GetMousePosition();
                CursorQuad.Position = mousePos;
            }

            if (Quad != null)
            {
                HandleMoveInput();
            }

            // Updated attached camera to player object
            if (Camera != null)
            {
                var camQuad = Camera.GetComponent<QuadComponent>();
                camQuad.Position = Quad.Position + new Vector2(-64, -36);
            }
        }

        public override void OnCollisionBegin(Collision collision)
        {
        }

        public override void OnCollisionEnd(Collision collision)
        {
        }

        private void HandleMoveInput()
        {
            if (Input.IsMousePressed(MouseCode.MOUSE_BUTTON_LEFT) || Input.IsMousePressed(MouseCode.MOUSE_BUTTON_RIGHT))
            {
                Vector2 mousePos = Input.GetMousePosition();
                Vector2 pos = Quad.Position;

                Vector2 dir = mousePos - pos;
                Vector2 vecVec = dir.Unit * Velocity;
                //Console.WriteLine($"SetVelocity - Player: {vecVec}");
                if (Input.IsMousePressed(MouseCode.MOUSE_BUTTON_LEFT))
                    Physics.SetLinearVelocity(ID, vecVec);
                else
                    Physics.SetLinearVelocity(ID, vecVec.Inv);
            }
            else
            {
                Physics.SetLinearVelocity(ID, Vector2.Zero);
            }

            // Spawn a bullet
            if (Input.IsKeyDown(KeyCode.KEY_SPACE))
            {
                if (_spellBar != null)
                {
                    ProjectileType projType = ProjectileType.FIRE_BALL;
                    Vector2 projSize = new Vector2(6.0f, 6.0f);
                    string pattern = _spellBar.ConsumeRegiseredKeyPattern();
                    if (pattern.Length > 0)
                    {
                        Vector2 mousePos = Input.GetMousePosition();

                        if (pattern.Contains("W"))
                        {
                            projType = ProjectileType.LIGHTNING_BOLT;
                            projSize = new Vector2(12.0f, 6.0f);
                            ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, mousePos, projSize));
                            return;
                        }

                        if (pattern == "QQQQ")
                        {
                            // Ring of fire balls 8 instances
                            projType = ProjectileType.FIRE_BALL;
                            Vector2 pos = Quad.Position;
                            ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, pos + new Vector2(150.0f, 0.0f), projSize));
                            ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, pos + new Vector2(-150.0f, 0.0f), projSize));
                            ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, pos + new Vector2(0.0f, 150.0f), projSize));
                            ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, pos + new Vector2(0.0f, -150.0f), projSize));
                            ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, pos + new Vector2(150.0f, 150.0f), projSize));
                            ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, pos + new Vector2(150.0f, -150.0f), projSize));
                            ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, pos + new Vector2(-150.0f, 150.0f), projSize));
                            ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, pos + new Vector2(-150.0f, -150.0f), projSize));
                            return;
                        }


                        ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, mousePos, projSize));
                    }
                }
            }
        }
    }
}
