using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

// TODO - script internal calls
// Get Window size or Camera view box

// TODO: consider - Circle of size(150,150) color(167,95,95,255) thickness 0.25 fade 0.888 - follow player to indicate low life

namespace Demo
{
    enum PlayerFacingDirection { UP = 0, DOWN, LEFT, RIGHT };
    enum PlayerState { IDLE = 0, WALK }

    [ScriptOrder(Priority = 1)]
    public class DemoPlayer : Entity
    {
        private QuadComponent _quad;
        private QuadComponent _cursorQuad;

        private AnimatedSpriteComponent _anim;
        private PlayerFacingDirection _facingDir = PlayerFacingDirection.DOWN;
        private PlayerState _playerState = PlayerState.IDLE;

        private SpellBar _spellBar;
        private HealthBar _healthBar;
        public float Health = 100.0f;

        public float Velocity = 20.0f;
        public Entity Camera;
        public Entity Cursor;
        public float Projectiles = 2.0f;

        public override void OnCreate()
        {
            // FIXME: This is temporary solution, add OnDestroy to scripts
            // Clear all instances in ObjectPool(s) on "Scene Start"
            EffectsPool.Clear();
            ProjectileFactory.Clear();
            EnemyPool.Clear();

            _quad = GetComponent<QuadComponent>();
            if (Cursor != null)
            {
                _cursorQuad = Cursor.GetComponent<QuadComponent>();
            }

            _anim = GetComponent<AnimatedSpriteComponent>();
            if (_anim == null)
                Console.WriteLine("Player - Animation component not set!");
            else
            {
                _anim.Play("unarmed_idle_front.anim");
                _anim.Looping = true;
            }

            // Grab spellBar ref, after all onCreates are done
            if (_spellBar == null)
            {
                var entity = FindEntityByName("UI_SpellBar");
                if (entity != null)
                {
                    _spellBar = entity.As<SpellBar>();
                }
            }
        }

        public override void OnUpdate(float ts)
        {
            // Check game over condition (Health)
            if (Health <= 0.0f)
            {
                if (_healthBar != null)
                    _healthBar.SetFillPercentage(Health / 100.0f);
                Console.WriteLine("Game Over! Go ahead and restart...");
                RigidBody2DComponent rb2d = GetComponent<RigidBody2DComponent>();
                rb2d.Enabled = false;
                return;
            }

            // Update HealthBar
            if (_healthBar == null)
            {
                // Note - atm we can't create new instances in OnCreate()
                // Create HealthBar entity and "attach"
                _healthBar = Entity.Create<HealthBar>();
                _healthBar.Initialize(this);
                _healthBar.SetColor(new Color(0.35f, 0.85f, 0.25f, 1.0f));
            }
            else
            {
                _healthBar.SetFillPercentage(Health / 100.0f);
                _healthBar.UpdatePosition();
            }

            // Tracking "dummy" object - just to show where the target actually is (cursor pos)
            Cursor = FindEntityByName("Cursor");
            if (Cursor != null)
            {
                _cursorQuad = Cursor.GetComponent<QuadComponent>();
                Vector2 mousePos = Input.GetMousePosition();
                _cursorQuad.Position = mousePos;
            }

            if (_quad != null)
            {
                HandleMoveInput();
            }
            
            // Updated attached camera to player object
            if (Camera != null)
            {
                var camQuad = Camera.GetComponent<QuadComponent>();
                camQuad.Position = _quad.Position + new Vector2(-64, -36);
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
                Vector2 pos = _quad.Position;

                Vector2 dir = mousePos - pos;
                Vector2 vecVec = dir.Unit * Velocity;
                //Console.WriteLine($"SetVelocity - Player: {vecVec}");
                if (Input.IsMousePressed(MouseCode.MOUSE_BUTTON_LEFT))
                {
                    UpdateFacingDir(dir.Unit, false);
                    Physics.SetLinearVelocity(ID, vecVec);
                }
                else
                {
                    UpdateFacingDir(dir.Unit, false);
                    Physics.SetLinearVelocity(ID, vecVec.Inv);
                }

            }
            else
            {
                Physics.SetLinearVelocity(ID, Vector2.Zero);
            }

            if (Input.IsMouseUp(MouseCode.MOUSE_BUTTON_LEFT) || Input.IsMouseUp(MouseCode.MOUSE_BUTTON_RIGHT))
            {
                ChangeToIdleAnimation();
            }

            // Spawn a bullet
            if (Input.IsKeyPressed(KeyCode.KEY_SPACE))
            {
                if (_spellBar != null)
                {
                    ProjectileType projType = ProjectileType.FIREBALL;
                    Vector2 projSize = new Vector2(9.0f, 9.0f);
                    string pattern = _spellBar.ConsumeRegiseredKeyPattern();
                    if (pattern.Length >= 0)
                    {
                        Vector2 mousePos = Input.GetMousePosition();

                        if (pattern.Contains("W"))
                        {
                            projType = ProjectileType.LIGHTNING_BOLT;
                            projSize = new Vector2(16.0f, 9.0f);
                            ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, mousePos, projSize));
                            return;
                        }

                        if (pattern == "QQQQ")
                        {
                            // Ring of fire balls 8 instances
                            projType = ProjectileType.FIREBALL;
                            Vector2 pos = _quad.Position;
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

                        for (int i = 1; i < Projectiles; i++)
                            ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, MathUtils.RotatePointAroundCenter(mousePos, _quad.Position, -0.2617 * i), projSize));
                        ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, mousePos, projSize));
                        for (int i = 1; i < Projectiles; i++)
                            ProjectileFactory.CreateProjectile(projType, new ProjSpawnParams(this, MathUtils.RotatePointAroundCenter(mousePos, _quad.Position, 0.2617 * i), projSize));
                    }
                }
            }
        }

        private void UpdateFacingDir(Vector2 dirVec, bool reverse)
        {
            float angle = Vector2.AxisX.Angle(dirVec);
            //Console.WriteLine($"Player angle to X axis: {angle}");

            PlayerFacingDirection prevDir = _facingDir;

            if (angle <= 0.78f)
            {
                if (reverse) _facingDir = PlayerFacingDirection.LEFT; else _facingDir = PlayerFacingDirection.RIGHT;
            }
            else if (0.78 < angle && angle <= 2.35f && dirVec.Y > 0)
            {
                if (reverse) _facingDir = PlayerFacingDirection.DOWN; else _facingDir = PlayerFacingDirection.UP;
            }
            else if (0.78 < angle && angle <= 2.35f && dirVec.Y <= 0)
            {
                if (reverse) _facingDir = PlayerFacingDirection.UP; else _facingDir = PlayerFacingDirection.DOWN;
            }
            else
            {
                if (reverse) _facingDir = PlayerFacingDirection.RIGHT; else _facingDir = PlayerFacingDirection.LEFT;
            }

            if (prevDir != _facingDir || _playerState == PlayerState.IDLE)  // Direction changed, change animation
            {
                ChangeToMoveAnimation();
            }

        }

        private void ChangeToMoveAnimation()
        {
            switch (_facingDir)
            {
                case PlayerFacingDirection.RIGHT:
                    _anim.Play("unarmed_walk_side_right.anim");
                    break;
                case PlayerFacingDirection.UP:
                    _anim.Play("unarmed_walk_back.anim");
                    break;
                case PlayerFacingDirection.DOWN:
                    _anim.Play("unarmed_walk_front.anim");
                    break;
                case PlayerFacingDirection.LEFT:
                    _anim.Play("unarmed_walk_side_left.anim");
                    break;
            }
            _anim.Looping = true;
            _playerState = PlayerState.WALK;
        }

        private void ChangeToIdleAnimation()
        {
            _anim.Play("unarmed_idle_front.anim");
            _anim.Looping = true;
            _playerState = PlayerState.IDLE;
        }

        public static void DamagePlayer(float damage)
        {
            Entity playerEntity = Entity.FindByName("Player");
            DemoPlayer player = playerEntity.As<DemoPlayer>();
            if (player != null)
            {
                player.Health -= damage;
            }
        }

        public static Vector2 GetPosition()
        {
            Entity playerEntity = Entity.FindByName("Player");
            if (playerEntity != null)
            {
                DemoPlayer player = playerEntity.As<DemoPlayer>();
                if (player != null)
                {
                    return player._quad.Position;
                }
            }

            return new Vector2(-9999.0f, -9999.0f);
        }
    }
}
