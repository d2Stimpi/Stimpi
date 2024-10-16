using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Stimpi;

namespace Demo
{
    enum EnemyFacingDirection { UP = 0, DOWN, LEFT, RIGHT };
    // Reload is attack cooldown period
    enum EnemyState { SPAWN_BEGIN = 0, SPAWN, IDLE, WALK, ATTACK, RELOAD, SIDE_STEP, DEAD };

    public class Enemy : Entity
    {
        private bool _enableUpdate = true;

        private Random _rnd = new Random();
        private EnemySpawn _spawnEntity;

        private QuadComponent _quad;
        private BoxCollider2DComponent _bc2d;
        private RigidBody2DComponent _rb2d;

        private AnimatedSpriteComponent _anim;
        private EnemyFacingDirection _facingDir = EnemyFacingDirection.DOWN;
        private EnemyState _enemyState = EnemyState.IDLE;

        private HealthBar _healthBar;

        // State based timer variables
        private float _spawnTimerElapsed = 0.0f;
        private float _idleTimerElapsed = 0.0f;
        private float _walkTimerElapsed = 0.0f;
        private float _reloadTimerElapsed = 0.0f;
        private float _sidestepTimerElapsed = 0.0f;
        private float _deadTimerElapsed = 0.0f;
        public float SpawnTimer = 0.3f;
        public float IdleTimer = 2.0f;
        public float WalkTimer = 0.0f;
        public float ReloadTimer = 1.0f;
        public float SideStepdTimer = 0.3f;
        public float DeadTimer = 2.0f;

        private Vector2 _activeVelocity = Vector2.Zero;
        private Vector2 _prevVelocity = Vector2.Zero;

        private float _aggroDistance = 50.0f;
        private float _health = 100.0f;
        public float Velocity = 20.0f;

        public float ChangeDirFrequency = 1.5f;

        private bool SimpleActionTimer(float ts, ref float timer, float value)
        {
            timer += ts;
            if (timer >= value)
            {
                timer = 0;
                return true;
            }

            return false;
        }

        private void SetVelocity(Vector2 velocity)
        {
            _prevVelocity = _activeVelocity;
            _activeVelocity = velocity;
        }

        public void Initialize(EnemySpawn spawn, Vector2 position)
        {
            _enableUpdate = true;
            _spawnEntity = spawn;
            _quad.Position = position;

            Physics.InitializePhysics2DBody(ID);
            _enemyState = EnemyState.SPAWN_BEGIN;

            _rb2d.Enabled = true;
            _rb2d.SetTransform(_quad.Position, _quad.Rotation);


            if (_healthBar == null)
            {
                // Create HealthBar entity and "attach"
                _healthBar = Entity.Create<HealthBar>();
            }

            _healthBar.Initialize(this);
            _health = 100;
        }

        public void Invalidate(Vector2 position)
        {
            _rb2d.Enabled = false;
            _rb2d.SetTransform(position, 0);
            _anim.Stop();
            _healthBar.UpdatePosition();
            _enableUpdate = false;
        }

        public override void OnCreate()
        {
            _quad = AddComponent<QuadComponent>();
            _quad.Size = new Vector2(24.0f, 24.0f);
            
            _anim = AddComponent<AnimatedSpriteComponent>();
            _anim.AddAnimation("animations\\char\\sword_idle_front.anim");
            _anim.AddAnimation("animations\\char\\sword_walk_back.anim");
            _anim.AddAnimation("animations\\char\\sword_walk_front.anim");
            _anim.AddAnimation("animations\\char\\sword_walk_side_left.anim");
            _anim.AddAnimation("animations\\char\\sword_walk_side_right.anim");
            _anim.AddAnimation("animations\\char\\sword_attack_back.anim");
            _anim.AddAnimation("animations\\char\\sword_attack_front.anim");
            _anim.AddAnimation("animations\\char\\sword_attack_side_left.anim");
            _anim.AddAnimation("animations\\char\\sword_attack_side_right.anim");
            _anim.AddAnimation("animations\\char\\sword_death_back.anim");
            _anim.AddAnimation("animations\\char\\sword_death_front.anim");
            _anim.AddAnimation("animations\\char\\sword_death_side_left.anim");
            _anim.AddAnimation("animations\\char\\sword_death_side_right.anim");

            _bc2d = AddComponent<BoxCollider2DComponent>();
            _bc2d.Size = new Vector2(0.1f, 0.12f);
            _bc2d.Offset = new Vector2(0.0f, -1.0f);
            _bc2d.Shape = Collider2DShape.BOX;

            _rb2d = AddComponent<RigidBody2DComponent>();
            _rb2d.Type = BodyType.DYNAMIC;
            _rb2d.FixedRotation = true;
        }

        public override void OnUpdate(float ts)
        {
            if (_enableUpdate)
            {
                if (_healthBar != null)
                    _healthBar.UpdatePosition();
                // Maintain correct velocity
                Physics.SetLinearVelocity(ID, _activeVelocity);

                UpdateEnemyState(ts);

                UpdateActiveAnimation();

                if (_enemyState == EnemyState.DEAD)
                {
                    if (SimpleActionTimer(ts, ref _deadTimerElapsed, DeadTimer))
                        DestroyEnemy();
                }
            }
        }

        public override void OnCollisionBegin(Collision collision)
        {
            Entity other = FindEntityByID(collision.OtherID);
            if (other != null)
            {
                string tag = other.GetComponent<TagComponent>().Tag;
                Console.WriteLine($"Collided with {tag}");
                if (tag == "Player")
                {
                    // Cause damage to Player
                    if (_enemyState != EnemyState.ATTACK/* && _enemyState != EnemyState.RELOAD*/)
                        DemoPlayer.DamagePlayer(10.0f);

                    _enemyState = EnemyState.ATTACK;
                    ChangeToAttackAnimation();
                }

                Projectile projectile = other.As<Projectile>();
                if (projectile != null) // If this is actually a Projectile type
                {
                    _health -= 25;
                    _healthBar.SetFillPercentage(_health / 100.0f);

                    if (_health <= 0)
                    {
                        _enemyState = EnemyState.DEAD;
                        ChangeToDeathAnimation();
                        _rb2d = GetComponent<RigidBody2DComponent>();
                        _rb2d.Enabled = false;
                    }
                    else
                    {
                        // If enemy is hit while idle, make it chase player
                        if (_enemyState == EnemyState.IDLE)
                            _enemyState = EnemyState.WALK;
                    }
                }
            }
        }

        public override void OnCollisionEnd(Collision collision)
        {
        }

        // Destroy or (TODO: pooling) disable entity
        private void DestroyEnemy()
        {
            _spawnEntity.EnemyKilled();
            Invalidate(new Vector2(-2000.0f, -2000.0f));
            EnemyPool.ReleaseObject(this);
        }

        private void UpdateActiveAnimation()
        {
            // Only skip checking animation if attacking
            if (_enemyState == EnemyState.ATTACK)
                return;

            if (_enemyState != EnemyState.DEAD)
            {
                if (_activeVelocity.Equals(Vector2.Zero))
                {
                    ChangeToIdleAnimation();
                }
                else
                {
                    UpdateFacingDir(_activeVelocity);
                }
            }
        }

        private void MoveTowards(Vector2 target)
        {
            Vector2 dir = target - _quad.Position;
            SetVelocity(dir.Unit * Velocity);
        }

        private void MoveStop()
        {
            SetVelocity(Vector2.Zero);
        }

        private void UpdateMoveDirection(float ts)
        {
            // Follow player on each dir change
            Entity playerEnt = FindEntityByName("Player");
            if (playerEnt != null)
            {
                //DemoPlayer player = playerEnt.As<DemoPlayer>();
                Vector2 playerPos = playerEnt.GetComponent<QuadComponent>().Position;
                Vector2 dir = playerPos - _quad.Position;

                SetVelocity(dir.Unit * Velocity);
                Console.WriteLine($"Enemy velocity: {_activeVelocity}");
            }
            else
            {
                Console.WriteLine("DemoPlayer not found");
            }
        }

        private void UpdateFacingDir(Vector2 dirVec)
        {
            float angle = Vector2.AxisX.Angle(dirVec);

            EnemyFacingDirection prevDir = _facingDir;

            if (angle <= 0.78f)
            {
                _facingDir = EnemyFacingDirection.RIGHT;
            }
            else if (0.78 < angle && angle <= 2.35f && dirVec.Y > 0)
            {
                _facingDir = EnemyFacingDirection.UP;
            }
            else if (0.78 < angle && angle <= 2.35f && dirVec.Y <= 0)
            {
                _facingDir = EnemyFacingDirection.DOWN;
            }
            else
            {
                _facingDir = EnemyFacingDirection.LEFT;
            }

            if (prevDir != _facingDir || _prevVelocity.Equal(Vector2.Zero))  // Direction changed, change animation
            {
                _prevVelocity = _activeVelocity;
                ChangeToMoveAnimation();
            }

        }

        private void ChangeToMoveAnimation()
        {
            switch (_facingDir)
            {
                case EnemyFacingDirection.RIGHT:
                    _anim.Play("sword_walk_side_right.anim");
                    break;
                case EnemyFacingDirection.UP:
                    _anim.Play("sword_walk_back.anim");
                    break;
                case EnemyFacingDirection.DOWN:
                    _anim.Play("sword_walk_front.anim");
                    break;
                case EnemyFacingDirection.LEFT:
                    _anim.Play("sword_walk_side_left.anim");
                    break;
            }
            _anim.Looping = true;
        }

        private void ChangeToAttackAnimation()
        {
            switch (_facingDir)
            {
                case EnemyFacingDirection.RIGHT:
                    _anim.Play("sword_attack_side_right.anim");
                    break;
                case EnemyFacingDirection.UP:
                    _anim.Play("sword_attack_back.anim");
                    break;
                case EnemyFacingDirection.DOWN:
                    _anim.Play("sword_attack_front.anim");
                    break;
                case EnemyFacingDirection.LEFT:
                    _anim.Play("sword_attack_side_left.anim");
                    break;
            }
            _anim.Looping = false;

            SetVelocity(Vector2.Zero);
        }

        private void ChangeToDeathAnimation()
        {
            switch (_facingDir)
            {
                case EnemyFacingDirection.RIGHT:
                    _anim.Play("sword_death_side_right.anim");
                    break;
                case EnemyFacingDirection.UP:
                    _anim.Play("sword_death_back.anim");
                    break;
                case EnemyFacingDirection.DOWN:
                    _anim.Play("sword_death_front.anim");
                    break;
                case EnemyFacingDirection.LEFT:
                    _anim.Play("sword_death_side_left.anim");
                    break;
            }
            _anim.Looping = false;
            _anim.WrapMode = AnimationWrapMode.CLAMP;

            SetVelocity(Vector2.Zero);
        }

        private void ChangeToIdleAnimation()
        {
            if (_anim.IsPlaying("sword_idle_front.anim") == false)
            {
                _anim.Play("sword_idle_front.anim");
                _anim.Looping = true;
            }
        }

        private void UpdateEnemyState(float ts)
        {
            switch(_enemyState)
            {
                case EnemyState.SPAWN_BEGIN:
                    Vector2 target = new Vector2(_rnd.Next((int)_quad.Position.X, (int)_quad.Position.X + 30),
                                                 _rnd.Next((int)_quad.Position.Y, (int)_quad.Position.Y + 30));
                    SpawnTimer = 0.3f + (float)_rnd.NextDouble() * 0.7f;
                    Console.WriteLine($" rnd move target {target}");
                    MoveTowards(target);

                    _enemyState = EnemyState.SPAWN;
                    break;
                case EnemyState.SPAWN:

                    //Console.WriteLine($"_spawnTimerElapsed {_spawnTimerElapsed}");
                    _spawnTimerElapsed += ts;
                    if (_spawnTimerElapsed >= SpawnTimer)
                    {
                        MoveStop();
                        _spawnTimerElapsed = 0.0f;
                        _enemyState = EnemyState.IDLE;
                    }
                    break;
                case EnemyState.IDLE:
                    // Check for player proximity and get "alerted" and chase player
                    Entity playerEnt = FindEntityByName("Player");
                    if (playerEnt != null)
                    {
                        Vector2 playerPos = playerEnt.GetComponent<QuadComponent>().Position;
                        if (_quad.Position.Distance(playerPos) <= _aggroDistance)
                        {
                            _enemyState = EnemyState.WALK;
                        }
                    }
                    break;
                case EnemyState.WALK:
                    _walkTimerElapsed += ts;
                    if (_walkTimerElapsed >= WalkTimer)
                    {
                        _walkTimerElapsed = 0;
                        UpdateMoveDirection(ts);
                        // Randomize next re-track timer
                        WalkTimer = 0.5f + ChangeDirFrequency * (float)_rnd.NextDouble();
                    }
                    break;
                /* Transition to ATTACK anim state is when colliding with Player object */
                case EnemyState.ATTACK:
                    if (_anim.AnimState == AnimationState.COMPELETED)
                    {
                        _enemyState = EnemyState.RELOAD;
                    }
                    break;
                case EnemyState.RELOAD:
                    _reloadTimerElapsed += ts;
                    if (_reloadTimerElapsed >= SpawnTimer)
                    {
                        _reloadTimerElapsed = 0.0f;
                        _enemyState = EnemyState.WALK;
                    }
                    break;
                case EnemyState.SIDE_STEP:
                    if (SimpleActionTimer(ts, ref _sidestepTimerElapsed, SideStepdTimer))
                    {
                        _enemyState = EnemyState.WALK;
                    }
                    break;

            }
        }
    }
}
